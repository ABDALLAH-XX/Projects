/* ============================================================
   ESP32-CAM + Edge Impulse
   Point d'accès WiFi autonome + page web (photo + prédictions)
   => Aucun PC/moniteur série nécessaire pour voir les résultats
   ============================================================ */

#include <Arduino.h>

#ifndef EI_PORTING_ARDUINO
#define EI_PORTING_ARDUINO 1
#endif

#include "abdallah-xx-project-1_inferencing.h"
#include "edge-impulse-sdk/dsp/image/image.hpp"
#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"
#include "base64.h"

/* ---------------- Selection du modele de camera ---------------- */
/* Decommente UNE seule des deux lignes suivantes selon la carte utilisee */
//#define CAMERA_MODEL_AI_THINKER   // ESP32-CAM classique
#define CAMERA_MODEL_ESP32S3_EYE    // Freenove ESP32-S3-WROOM CAM

#if defined(CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22
#define CAM_XCLK_FREQ_HZ  20000000
#define CAM_FB_COUNT      2
#define CAM_JPEG_QUALITY  12

#elif defined(CAMERA_MODEL_ESP32S3_EYE)
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     15
#define SIOD_GPIO_NUM      4
#define SIOC_GPIO_NUM      5
#define Y9_GPIO_NUM       16
#define Y8_GPIO_NUM       17
#define Y7_GPIO_NUM       18
#define Y6_GPIO_NUM       12
#define Y5_GPIO_NUM       10
#define Y4_GPIO_NUM        8
#define Y3_GPIO_NUM        9
#define Y2_GPIO_NUM       11
#define VSYNC_GPIO_NUM     6
#define HREF_GPIO_NUM      7
#define PCLK_GPIO_NUM     13
#define CAM_XCLK_FREQ_HZ  10000000
#define CAM_FB_COUNT      1
#define CAM_JPEG_QUALITY  10

#else
#error "Camera model not selected"
#endif

/* ---------------- Wi-Fi : point d'accès autonome ---------------- */
const char* ap_ssid     = "ESP32-CAM-CAMERAS";
const char* ap_password = "Password1234";

IPAddress local_IP(192, 168, 4, 1);
IPAddress gateway(192, 168, 4, 1);
IPAddress subnet(255, 255, 255, 0);

httpd_handle_t camera_httpd = NULL;

/* ---------------- Constantes caméra / inférence ---------------- */
#define EI_CAMERA_RAW_FRAME_BUFFER_COLS   320
#define EI_CAMERA_RAW_FRAME_BUFFER_ROWS   240
#define EI_CAMERA_FRAME_BYTE_SIZE         3

static bool debug_nn = false;
static bool is_initialised = false;
uint8_t *snapshot_buf = nullptr;

static camera_config_t camera_config = {
    .pin_pwdn       = PWDN_GPIO_NUM,
    .pin_reset      = RESET_GPIO_NUM,
    .pin_xclk       = XCLK_GPIO_NUM,
    .pin_sscb_sda   = SIOD_GPIO_NUM,
    .pin_sscb_scl   = SIOC_GPIO_NUM,
    .pin_d7 = Y9_GPIO_NUM, .pin_d6 = Y8_GPIO_NUM, .pin_d5 = Y7_GPIO_NUM, .pin_d4 = Y6_GPIO_NUM,
    .pin_d3 = Y5_GPIO_NUM, .pin_d2 = Y4_GPIO_NUM, .pin_d1 = Y3_GPIO_NUM, .pin_d0 = Y2_GPIO_NUM,
    .pin_vsync = VSYNC_GPIO_NUM,
    .pin_href  = HREF_GPIO_NUM,
    .pin_pclk  = PCLK_GPIO_NUM,
    .xclk_freq_hz = CAM_XCLK_FREQ_HZ,
    .ledc_timer   = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,
    .pixel_format = PIXFORMAT_JPEG,
    .frame_size   = FRAMESIZE_QVGA,
    .jpeg_quality = CAM_JPEG_QUALITY,
    .fb_count     = CAM_FB_COUNT,
    .fb_location  = CAMERA_FB_IN_PSRAM,
    .grab_mode    = CAMERA_GRAB_WHEN_EMPTY,
};

/* ---------------- Prototypes ---------------- */
bool ei_camera_init(void);
void ei_camera_deinit(void);
bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf);
static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr);
void startCameraServer();

/* ============================================================
   Page HTML servie à la racine ("/") : photo + résultats
   ============================================================ */
static const char INDEX_HTML[] PROGMEM = R"====(
<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <title>ESP32-CAM - Inference</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: sans-serif; text-align: center; background:#111; color:#eee; }
    img { max-width: 95%; border: 2px solid #444; border-radius: 8px; margin-top: 10px; }
    pre { text-align: left; display:inline-block; background:#222; padding:10px; border-radius:8px; }
  </style>
</head>
<body>
  <h2>ESP32-CAM &mdash; Detection en direct</h2>
  <img id="cam" src="">
  <br>
  <pre id="results">En attente de la premiere inference (peut prendre quelques secondes)...</pre>
  <script>
    const MARKER = "##EI-SPLIT##";
    function refreshAll() {
      fetch('/predict').then(r => r.text()).then(t => {
        const idx = t.indexOf(MARKER);
        if (idx === -1) {
          document.getElementById('results').innerText = t;
        } else {
          const b64 = t.substring(0, idx);
          const txt = t.substring(idx + MARKER.length);
          document.getElementById('cam').src = 'data:image/jpeg;base64,' + b64;
          document.getElementById('results').innerText = txt;
        }
      }).catch(e => {
        document.getElementById('results').innerText = 'Erreur reseau, nouvelle tentative...';
      }).finally(() => {
        // On relance seulement une fois le cycle precedent termine
        setTimeout(refreshAll, 200);
      });
    }
    refreshAll();
  </script>
</body>
</html>
)====";

static esp_err_t index_handler(httpd_req_t *req) {
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, INDEX_HTML, strlen(INDEX_HTML));
}

/* ============================================================
   Handler /predict : UNE SEULE capture, reutilisee pour
   l'affichage (JPEG -> base64) ET l'inference Edge Impulse.
   Reponse : <image base64>##EI-SPLIT##<texte des resultats>
   ============================================================ */
static const char* SPLIT_MARKER = "##EI-SPLIT##";

static esp_err_t predict_handler(httpd_req_t *req) {
    uint32_t t_start = millis();

    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    snapshot_buf = (uint8_t*)malloc(EI_CAMERA_RAW_FRAME_BUFFER_COLS * EI_CAMERA_RAW_FRAME_BUFFER_ROWS * EI_CAMERA_FRAME_BYTE_SIZE);
    if (snapshot_buf == nullptr) {
        esp_camera_fb_return(fb);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);

    // On encode le JPEG BRUT original (avant conversion) pour l'affichage,
    // puis on rend le buffer camera : plus besoin de refaire une capture.
    String imgB64 = base64::encode(fb->buf, fb->len);
    esp_camera_fb_return(fb);

    char resultText[768];
    int len = 0;

    if (!converted) {
        free(snapshot_buf);
        len = snprintf(resultText, sizeof(resultText), "ERR: conversion echouee\n");
    } else {
        if ((EI_CLASSIFIER_INPUT_WIDTH != EI_CAMERA_RAW_FRAME_BUFFER_COLS) ||
            (EI_CLASSIFIER_INPUT_HEIGHT != EI_CAMERA_RAW_FRAME_BUFFER_ROWS)) {
            ei::image::processing::crop_and_interpolate_rgb888(
                snapshot_buf, EI_CAMERA_RAW_FRAME_BUFFER_COLS, EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
                snapshot_buf, EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
        }

        ei::signal_t signal;
        signal.total_length = EI_CLASSIFIER_INPUT_WIDTH * EI_CLASSIFIER_INPUT_HEIGHT;
        signal.get_data = &ei_camera_get_data;

        ei_impulse_result_t result = { 0 };
        EI_IMPULSE_ERROR err = run_classifier(&signal, &result, debug_nn);
        free(snapshot_buf);

        if (err != EI_IMPULSE_OK) {
            len = snprintf(resultText, sizeof(resultText), "ERR: classifier (%d)\n", err);
        } else {
            len += snprintf(resultText + len, sizeof(resultText) - len,
                "DSP: %d ms | Classification: %d ms | Anomaly: %d ms\n",
                result.timing.dsp, result.timing.classification, result.timing.anomaly);

#if EI_CLASSIFIER_OBJECT_DETECTION == 1
            len += snprintf(resultText + len, sizeof(resultText) - len, "Objets detectes:\n");
            for (uint32_t i = 0; i < result.bounding_boxes_count && len < (int)sizeof(resultText) - 80; i++) {
                ei_impulse_result_bounding_box_t bb = result.bounding_boxes[i];
                if (bb.value == 0) continue;
                len += snprintf(resultText + len, sizeof(resultText) - len,
                    "  %s (%.2f) x:%u y:%u w:%u h:%u\n",
                    bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
            }
#else
            len += snprintf(resultText + len, sizeof(resultText) - len, "Predictions:\n");
            for (uint16_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT && len < (int)sizeof(resultText) - 60; i++) {
                len += snprintf(resultText + len, sizeof(resultText) - len,
                    "  %s: %.5f\n", ei_classifier_inferencing_categories[i], result.classification[i].value);
            }
#endif

#if EI_CLASSIFIER_HAS_ANOMALY
            len += snprintf(resultText + len, sizeof(resultText) - len, "Anomalie: %.3f\n", result.anomaly);
#endif
        }
    }

    uint32_t t_total = millis() - t_start;
    ei_printf("Cycle complet: %lu ms (%.1f FPS)\n", (unsigned long)t_total, 1000.0f / t_total);
    len += snprintf(resultText + len, sizeof(resultText) - len,
        "\nCycle complet: %lu ms (%.1f FPS)\n", (unsigned long)t_total, 1000.0f / t_total);

    httpd_resp_set_type(req, "text/plain");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send_chunk(req, imgB64.c_str(), imgB64.length());
    httpd_resp_send_chunk(req, SPLIT_MARKER, strlen(SPLIT_MARKER));
    httpd_resp_send_chunk(req, resultText, len);
    httpd_resp_send_chunk(req, NULL, 0);

    return ESP_OK;
}

/* ============================================================
   Démarrage du serveur HTTP
   ============================================================ */
void startCameraServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;
    config.stack_size = 16384; // l'inference a besoin de bien plus de pile que la valeur par defaut

    httpd_uri_t index_uri = {
        .uri = "/", .method = HTTP_GET, .handler = index_handler, .user_ctx = NULL
    };
    httpd_uri_t predict_uri = {
        .uri = "/predict", .method = HTTP_GET, .handler = predict_handler, .user_ctx = NULL
    };

    if (httpd_start(&camera_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(camera_httpd, &index_uri);
        httpd_register_uri_handler(camera_httpd, &predict_uri);
    }
}

/* ============================================================
   setup() / loop()
   ============================================================ */
void setup() {
    Serial.begin(115200);

    if (!ei_camera_init()) {
        Serial.println("Erreur d'initialisation de la camera");
        return;
    }
    Serial.println("Camera initialisee");

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP, gateway, subnet);
    WiFi.softAP(ap_ssid, ap_password);

    Serial.println("Point d'acces WiFi pret !");
    Serial.print("Connecte-toi au reseau '");
    Serial.print(ap_ssid);
    Serial.println("' puis ouvre dans un navigateur :");
    Serial.print("http://");
    Serial.println(WiFi.softAPIP());

    startCameraServer();
}

void loop() {
    delay(10000); // tout se passe dans les handlers HTTP, rien a faire ici
}

/* ============================================================
   Fonctions caméra Edge Impulse (capture -> RGB888 -> resize)
   ============================================================ */
bool ei_camera_init(void) {
    if (is_initialised) return true;

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        Serial.printf("Camera init failed with error 0x%x\n", err);
        return false;
    }

    sensor_t * s = esp_camera_sensor_get();
    uint16_t pid = s->id.PID;
    if (pid == OV2640_PID) {
        s->set_hmirror(s, 1);
        s->set_vflip(s, 1);
    } else if (pid == OV3660_PID) {
        s->set_hmirror(s, 1);
        s->set_vflip(s, 1);  // corrige: image inversee sur ce montage
    } else if (pid == GC2145_PID) {
        s->set_hmirror(s, 0);
        delay(500);
        s->set_vflip(s, 0);
    } else if (pid == GC0308_PID) {
        s->set_hmirror(s, 0);
        delay(500);
        s->set_vflip(s, 0);
    } else {
        s->set_hmirror(s, 1);
        s->set_vflip(s, 0);
    }
    s->set_brightness(s, 1);
    s->set_saturation(s, 0);
    s->set_ae_level(s, -3);

    is_initialised = true;
    return true;
}

void ei_camera_deinit(void) {
    esp_err_t err = esp_camera_deinit();
    if (err != ESP_OK) {
        ei_printf("Camera deinit failed\n");
        return;
    }
    is_initialised = false;
}

bool ei_camera_capture(uint32_t img_width, uint32_t img_height, uint8_t *out_buf) {
    bool do_resize = false;

    if (!is_initialised) {
        ei_printf("ERR: Camera is not initialized\r\n");
        return false;
    }

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        ei_printf("Camera capture failed\n");
        return false;
    }

    bool converted = fmt2rgb888(fb->buf, fb->len, PIXFORMAT_JPEG, snapshot_buf);
    esp_camera_fb_return(fb);

    if (!converted) {
        ei_printf("Conversion failed\n");
        return false;
    }

    if ((img_width != EI_CAMERA_RAW_FRAME_BUFFER_COLS) || (img_height != EI_CAMERA_RAW_FRAME_BUFFER_ROWS)) {
        do_resize = true;
    }

    if (do_resize) {
        ei::image::processing::crop_and_interpolate_rgb888(
            out_buf, EI_CAMERA_RAW_FRAME_BUFFER_COLS, EI_CAMERA_RAW_FRAME_BUFFER_ROWS,
            out_buf, img_width, img_height);
    }

    return true;
}

static int ei_camera_get_data(size_t offset, size_t length, float *out_ptr) {
    size_t pixel_ix = offset * 3;
    size_t pixels_left = length;
    size_t out_ptr_ix = 0;

    while (pixels_left != 0) {
        out_ptr[out_ptr_ix] = (snapshot_buf[pixel_ix + 2] << 16) + (snapshot_buf[pixel_ix + 1] << 8) + snapshot_buf[pixel_ix];
        out_ptr_ix++;
        pixel_ix += 3;
        pixels_left--;
    }
    return 0;
}

#if !defined(EI_CLASSIFIER_SENSOR) || EI_CLASSIFIER_SENSOR != EI_CLASSIFIER_SENSOR_CAMERA
#error "Invalid model for current sensor"
#endif