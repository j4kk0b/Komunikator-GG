#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLIENT 2
#define BUFFER_SIZE 1024

typedef struct {
    int sock;
    char username[32];
    char unread_messages[BUFFER_SIZE]; // Przechowuje nieprzeczytane wiadomości
} Client;

Client clients[MAX_CLIENT];
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Funkcja obsługująca połączenie dla każdego klienta
void *connection_handler(void *);

int main(int argc, char *argv[])
{
    int socket_desc, client_sock, c;
    struct sockaddr_in server, client;
    pthread_t thread_id;

    // Tworzenie gniazda
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        printf("Nie mogę utworzyć gniazda");
        return 1;
    }

    // Konfiguracja serwera (adres i port)
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(9000);

    // Bindowanie gniazda do adresu i portu
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("błąd bind");
        return 1;
    }

    // Nasłuchiwanie na połączenia
    listen(socket_desc, MAX_CLIENT);

    printf("Oczekiwanie na połączenia...\n");
    c = sizeof(struct sockaddr_in);

    while ((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)))
    {
        // Komunikat o zaakceptowanym połączeniu
        printf("Połączenie zaakceptowane od %s\n", inet_ntoa(client.sin_addr));

        // Tworzenie wątku obsługującego połączenie
        if (pthread_create(&thread_id, NULL, connection_handler, (void *)&client_sock) < 0)
        {
            perror("Nie udało się utworzyć wątku");
            return 1;
        }
    }

    if (client_sock < 0)
    {
        perror("błąd akceptacji");
        return 1;
    }

    return 0;
}

// Funkcja obsługująca połączenie dla każdego klienta
void *connection_handler(void *socket_desc)
{
    int sock = *(int *)socket_desc;
    int read_size;
    char client_message[BUFFER_SIZE];

    // Odbieranie pierwszej wiadomości od klienta
    read_size = recv(sock, client_message, BUFFER_SIZE, 0);
    if (read_size <= 0)
    {
        perror("Błąd odbierania");
        return 0;
    }

    // Rejestracja klienta i jego nazwy użytkownika
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (clients[i].sock == 0)
        {
            clients[i].sock = sock;
            strncpy(clients[i].username, client_message, sizeof(clients[i].username));
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    // Obsługa nieprzeczytanych wiadomości
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (clients[i].sock != 0 && strcmp(clients[i].username, client_message) == 0)
        {
            if (strlen(clients[i].unread_messages) > 0) {
                // Jeśli są nieprzeczytane wiadomości, wysyłamy je
                write(sock, clients[i].unread_messages, strlen(clients[i].unread_messages));
                memset(clients[i].unread_messages, 'X', BUFFER_SIZE); // Czyszczenie nieprzeczytanych wiadomości
            }
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    // Pętla odbierająca i przesyłająca wiadomości między klientami
    while ((read_size = recv(sock, client_message, BUFFER_SIZE, 0)) > 0)
    {
        char *recipient = strtok(client_message, "-");
        char *message = strtok(NULL, "");

        pthread_mutex_lock(&mutex);
        for (int i = 0; i < MAX_CLIENT; i++)
        {
            if (clients[i].sock != 0 && strcmp(clients[i].username, recipient) == 0)
            {
                write(clients[i].sock, message, strlen(message));
                for(int i = 0; i < BUFFER_SIZE; i++)
                {
                    message[i] = '\0';
                }
                break;
            }
            else if (strcmp(clients[i].username, recipient) == 0)
            {
                // Przechowujemy nieprzeczytaną wiadomość
                strcat(clients[i].unread_messages, message);
                for(int i = 0; i < BUFFER_SIZE; i++)
                {
                    message[i] = '\0';
                }
                strcat(clients[i].unread_messages, "\n"); // Oddzielamy wiadomości nową linią
                break;
            }
        }
        pthread_mutex_unlock(&mutex);
    }

    // Zamykanie połączenia i usuwanie klienta po jego rozłączeniu
    if (read_size == 0)
    {
        printf("Klient rozłączony\n");
        fflush(stdout);
    }
    else if (read_size == -1)
    {
        perror("Błąd odbierania");
    }

    pthread_mutex_lock(&mutex);
    for (int i = 0; i < MAX_CLIENT; i++)
    {
        if (clients[i].sock == sock)
        {
            clients[i].sock = 0;
            break;
        }
    }
    pthread_mutex_unlock(&mutex);

    return 0;
}