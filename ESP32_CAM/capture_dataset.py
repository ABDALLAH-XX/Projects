import os
import time
import threading
import requests
import io
import tkinter as tk
from tkinter import ttk, messagebox
from PIL import Image, ImageTk

class CollectorGUIWithoutCV2:
    def __init__(self, root):
        self.root = root
        self.root.title("Collecteur ESP32-CAM (Sans OpenCV)")
        self.root.geometry("450x400")

        self.is_capturing = False
        self.is_streaming = False
        self.image_count = 0

        # --- Panneau Contrôles ---
        left_frame = tk.Frame(root)
        left_frame.pack(side='left', fill='y', padx=15, pady=15)

        tk.Label(left_frame, text="IP ESP32-CAM :", font=('Arial', 9, 'bold')).pack(anchor='w', pady=(0, 2))
        self.ip_entry = tk.Entry(left_frame, font=('Arial', 9))
        self.ip_entry.insert(0, "192.168.4.1")
        self.ip_entry.pack(fill='x', pady=(0, 10))

        tk.Label(left_frame, text="Label / Classe :", font=('Arial', 9, 'bold')).pack(anchor='w', pady=(0, 2))
        self.label_entry = tk.Entry(left_frame, font=('Arial', 9))
        self.label_entry.insert(0, "objet_a")
        self.label_entry.pack(fill='x', pady=(0, 10))

        tk.Label(left_frame, text="Intervalle (sec) :", font=('Arial', 9, 'bold')).pack(anchor='w', pady=(0, 2))
        self.interval_spinbox = ttk.Spinbox(left_frame, from_=0.2, to=10.0, increment=0.2, font=('Arial', 9))
        self.interval_spinbox.set(1.0)
        self.interval_spinbox.pack(fill='x', pady=(0, 15))

        self.stream_btn = tk.Button(left_frame, text="🎥 Aperçu", bg="#17a2b8", fg="white", font=('Arial', 10, 'bold'), command=self.toggle_stream)
        self.stream_btn.pack(fill='x', pady=5)

        self.start_btn = tk.Button(left_frame, text="▶ Enregistrer", bg="#28a745", fg="white", font=('Arial', 10, 'bold'), command=self.start_capture)
        self.start_btn.pack(fill='x', pady=5)

        self.stop_btn = tk.Button(left_frame, text="⬛ Stopper", bg="#dc3545", fg="white", font=('Arial', 10, 'bold'), state='disabled', command=self.stop_capture)
        self.stop_btn.pack(fill='x', pady=5)

        self.counter_label = tk.Label(left_frame, text="Images enregistrées : 0", font=('Arial', 10, 'bold'))
        self.counter_label.pack(pady=15)

        # --- Panneau Ecran Video ---
        right_frame = tk.Frame(root, bg="black")
        right_frame.pack(side='right', expand=True, fill='both', padx=10, pady=15)

        self.video_label = tk.Label(right_frame, text="Pas de flux", bg="black", fg="white")
        self.video_label.pack(expand=True, fill='both')

    def toggle_stream(self):
        if not self.is_streaming:
            self.is_streaming = True
            self.stream_btn.config(text="⏹ Stopper Aperçu", bg="#6c757d")
            threading.Thread(target=self.stream_loop, daemon=True).start()
        else:
            self.is_streaming = False
            self.stream_btn.config(text="🎥 Aperçu", bg="#17a2b8")
            self.video_label.config(image='', text="Pas de flux")

    def stream_loop(self):
        ip = self.ip_entry.get().strip()
        url = f"http://{ip}/capture"

        while self.is_streaming:
            try:
                res = requests.get(url, timeout=2)
                if res.status_code == 200:
                    img = Image.open(io.BytesIO(res.content))
                    img = img.resize((400, 300))
                    imgtk = ImageTk.PhotoImage(image=img)
                    self.video_label.imgtk = imgtk
                    self.video_label.configure(image=imgtk)
            except Exception:
                pass
            time.sleep(0.1)

    def start_capture(self):
        ip = self.ip_entry.get().strip()
        label = self.label_entry.get().strip()

        if not ip or not label:
            messagebox.showwarning("Attention", "Remplissez l'IP et le label.")
            return

        output_dir = os.path.join("dataset", label)
        os.makedirs(output_dir, exist_ok=True)

        self.is_capturing = True
        self.start_btn.config(state='disabled')
        self.stop_btn.config(state='normal')

        threading.Thread(target=self.capture_loop, args=(ip, output_dir, label), daemon=True).start()

    def capture_loop(self, ip, output_dir, label):
        url = f"http://{ip}/capture"
        interval = float(self.interval_spinbox.get())

        while self.is_capturing:
            try:
                response = requests.get(url, timeout=3)
                if response.status_code == 200:
                    timestamp = int(time.time() * 1000)
                    filepath = os.path.join(output_dir, f"{label}_{timestamp}.jpg")

                    with open(filepath, 'wb') as f:
                        f.write(response.content)

                    self.image_count += 1
                    self.counter_label.config(text=f"Images enregistrées : {self.image_count}")
            except Exception:
                pass

            time.sleep(interval)

    def stop_capture(self):
        self.is_capturing = False
        self.start_btn.config(state='normal')
        self.stop_btn.config(state='disabled')

if __name__ == "__main__":
    root = tk.Tk()
    app = CollectorGUIWithoutCV2(root)
    root.mainloop()
