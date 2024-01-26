= Temat zadania =
Projekt dotyczy stworzenia prostego komunikatora typu "GG" (Gadu-Gadu) w dwóch modułach: klienta i servera. 
Klient umożliwia użytkownikowi wysyłanie i odbieranie wiadomości tekstowych, natomiast server zarządza połączeniami między klientami oraz przekazuje im wiadomości.


= Opis protokołu komunikacyjnego =
Protokół komunikacyjny oparty jest na gniazdach TCP/IP.
Klient nawiązuje połączenie z serverem, wysyła swoją nazwę użytkownika oraz wiadomości do servera, który następnie przekazuje je odpowiednim klientom.


= Opis implementacji =
Klient (Python, Tkinter):
• Interfejs klienta jest oparty na bibliotece Tkinter.
• Klient pozwala użytkownikowi wprowadzić nazwę użytkownika, adres IP oraz port servera.
• Po nawiązaniu połączenia, klient może wysyłać i odbierać wiadomości od innych klientów.
• Klient obsługuje wątki do odbierania wiadomości asynchronicznie.

Server (C):
• Server został zaimplementowany w języku C.
• Server nasłuchuje na określonym porcie i akceptuje połączenia od klientów.
• Obsługa wielu klientów realizowana jest poprzez wątki.
• Server przechowuje listę klientów oraz ich nieprzeczytane wiadomości.


= Opis zawartości plików źródłowych =
Klient:
• client.py: Implementacja klienta w języku Python przy użyciu biblioteki Tkinter.

Server:
• server.c: Implementacja servera w języku C.


= Sposób kompilacji, uruchomienia i obsługi programów projektu =
Klient:
1. Uruchomienie klienta: python client.py.
2. Wprowadzenie nazwy użytkownika, adresu IP oraz portu servera.
3. Połączenie z serverem: kliknięcie przycisku "Połącz".
4. Wprowadzenie adresata i wiadomości, a następnie kliknięcie przycisku "Wyślij" dla wysłania wiadomości.
5. Rozłączenie klienta: kliknięcie przycisku "Rozłącz".

Server:
1. Kompilacja serwera: gcc server.c -o server
2. Uruchomienie servera: ./server
3. Serwer będzie nasłuchiwał na połączenia i obsługiwał komunikację między klientami.


= Podsumowanie =
Projekt "Komunikator GG" obejmuje implementację prostego systemu komunikacji opartego na gniazdach TCP/IP. 
Klient umożliwia użytkownikowi wysyłanie i odbieranie wiadomości, natomiast server zarządza połączeniami między klientami oraz przekazuje im wiadomości. 
Dzięki zastosowaniu wątków, aplikacja klienta może obsługiwać wiele połączeń jednocześnie, zapewniając płynną i efektywną komunikację.

