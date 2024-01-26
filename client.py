import tkinter as tk
from tkinter import messagebox
import socket
import threading

class ClientGUI:
    def __init__(self, master):
        # Inicjalizacja GUI klienta
        self.master = master
        master.title("Komunikator GG")

        # Interfejs użytkownika
        self.label_username = tk.Label(master, text="Nazwa użytkownika:")
        self.label_username.pack()

        self.entry_username = tk.Entry(master)
        self.entry_username.pack()

        self.label_server_ip = tk.Label(master, text="Server IP:")
        self.label_server_ip.pack()

        self.entry_server_ip = tk.Entry(master)
        self.entry_server_ip.pack()

        self.label_server_port = tk.Label(master, text="Server Port:")
        self.label_server_port.pack()

        self.entry_server_port = tk.Entry(master)
        self.entry_server_port.pack()

        self.connect_button = tk.Button(master, text="Połącz", command=self.connect_to_server)
        self.connect_button.pack()

        self.disconnect_button = tk.Button(master, text="Rozłącz", command=self.disconnect_from_server, state=tk.DISABLED)
        self.disconnect_button.pack()

        self.label_recipient = tk.Label(master, text="Odbiorca:")
        self.label_recipient.pack()

        self.entry_recipient = tk.Entry(master)
        self.entry_recipient.pack()

        self.label_message = tk.Label(master, text="Treść wiadomości:")
        self.label_message.pack()

        self.entry_message = tk.Entry(master)
        self.entry_message.pack()

        self.send_button = tk.Button(master, text="Send", command=self.send_message, state=tk.DISABLED)
        self.send_button.pack()

        self.text_display = tk.Text(master, height=10, width=50)
        self.text_display.pack()

        # Zmienna przechowująca obiekt klienta
        self.client_socket = None

    def connect_to_server(self):
        # Funkcja odpowiedzialna za nawiązywanie połączenia z serwerem
        username = self.entry_username.get()
        server_ip = self.entry_server_ip.get()
        server_port = int(self.entry_server_port.get())

        try:
            # Inicjalizacja gniazda klienta i połączenie z serwerem
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.connect((server_ip, server_port))
            self.client_socket.send(username.encode('utf-8'))

            # Konfiguracja przycisków po nawiązaniu połączenia
            self.connect_button.config(state=tk.DISABLED)
            self.disconnect_button.config(state=tk.NORMAL)
            self.send_button.config(state=tk.NORMAL)

            # Uruchomienie wątku odbierającego wiadomości od serwera
            self.receive_thread = threading.Thread(target=self.receive_messages)
            self.receive_thread.start()

        except Exception as e:
            messagebox.showerror("Error", str(e))

    def disconnect_from_server(self):
        # Funkcja odpowiedzialna za rozłączanie z serwerem
        if self.client_socket:
            self.client_socket.close()
        self.connect_button.config(state=tk.NORMAL)
        self.disconnect_button.config(state=tk.DISABLED)
        self.send_button.config(state=tk.DISABLED)

    def send_message(self):
        # Funkcja odpowiedzialna za wysyłanie wiadomości do serwera
        recipient = self.entry_recipient.get()
        message = self.entry_message.get()

        if not message:  # Sprawdzenie, czy wiadomość jest pusta
            messagebox.showwarning("Pusta wiadomość", "Nie możesz wysłać wiadomości, bez treści.")
            return

        # Formatowanie pełnej wiadomości z odbiorcą
        full_message = f"{recipient}-{message}"
        self.client_socket.send(full_message.encode('utf-8'))
        self.text_display.insert(tk.END, f"You: {message}\n")

        # Wyczyszczenie pola wprowadzania wiadomości po wysłaniu
        self.entry_message.delete(0, tk.END)

    def receive_messages(self):
        # Funkcja odpowiedzialna za ciągłe odbieranie wiadomości od serwera
        while True:
            try:
                message = self.client_socket.recv(1024).decode('utf-8')
                self.text_display.insert(tk.END, f"{message}\n")
            except:
                break

if __name__ == "__main__":
    root = tk.Tk()
    client_gui = ClientGUI(root)
    root.mainloop()