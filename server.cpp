#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>

/* portul folosit */
#define PORT 2028
int player_id = 0;
int *connected; // vectori jucatori deconectati
int *connected_count;
int *disconnected_count;
int *turn;
int *won;
int (*table)[11]; // matrice mapata in memorie care constituie harta
sem_t *client_sync;

struct pawn_state
{
	int moved;
	const int max_moved = 44;
} state[4];

struct move_coords_struct
{
	int x, y;
} move_coords[45];

void set_corner(int table[11][11], int x, int y, int value)
{
	table[x][y] = value;
	table[x][y + 1] = value;
	table[x + 1][y] = value;
	table[x + 1][y + 1] = value;
}

void set_short_vertical_line(int table[11][11], int x, int y)
{
	table[x][y] = 0;
	table[x][y + 1] = 0;
	table[x][y + 2] = 0;
}

void set_long_vertical_line(int table[11][11], int x, int y)
{
	table[x][y] = 0;
	table[x][y + 1] = 0;
	table[x][y + 2] = 0;
	table[x][y + 3] = 0;
	table[x][y + 4] = 0;
}

void set_short_horizontal_line(int table[11][11], int x, int y)
{
	table[x][y] = 0;
	table[x + 1][y] = 0;
	table[x + 2][y] = 0;
}

void set_long_horizontal_line(int table[11][11], int x, int y)
{
	table[x][y] = 0;
	table[x + 1][y] = 0;
	table[x + 2][y] = 0;
	table[x + 3][y] = 0;
	table[x + 4][y] = 0;
}

void table_setup(int table[11][11])
{
	for (int i = 0; i < 11; i++)
		for (int j = 0; j < 11; j++)
			table[i][j] = -1;

	set_corner(table, 9, 0, 1);
	set_corner(table, 0, 0, 2);
	set_corner(table, 0, 9, 3);
	set_corner(table, 9, 9, 4);

	set_short_vertical_line(table, 0, 4);
	set_short_vertical_line(table, 10, 4);
	set_short_horizontal_line(table, 4, 0);
	set_short_horizontal_line(table, 4, 10);

	set_long_horizontal_line(table, 0, 4);
	set_long_horizontal_line(table, 0, 5);
	set_long_horizontal_line(table, 0, 6);

	set_long_horizontal_line(table, 6, 4);
	set_long_horizontal_line(table, 6, 5);
	set_long_horizontal_line(table, 6, 6);

	set_long_vertical_line(table, 4, 0);
	set_long_vertical_line(table, 5, 0);
	set_long_vertical_line(table, 6, 0);

	set_long_vertical_line(table, 4, 6);
	set_long_vertical_line(table, 5, 6);
	set_long_vertical_line(table, 6, 6);
}

void move_coords_setup()
{
	for (int i = 0; i <= 4; i++)
		move_coords[i].x = 4, move_coords[i].y = 10 - i;
	for (int i = 5; i < 9; i++)
		move_coords[i].x = 8 - i, move_coords[i].y = 6;
	move_coords[9].x = 0, move_coords[9].y = 5;
	for (int i = 10; i < 15; i++)
		move_coords[i].x = i - 10, move_coords[i].y = 4;
	for (int i = 15; i < 19; i++)
		move_coords[i].x = 4, move_coords[i].y = 18 - i;
	move_coords[19].x = 5, move_coords[19].y = 0;
	for (int i = 20; i < 25; i++)
		move_coords[i].x = 6, move_coords[i].y = i - 20;
	for (int i = 25; i < 29; i++)
		move_coords[i].x = i - 18, move_coords[i].y = 4;
	move_coords[29].x = 10, move_coords[29].y = 5;
	for (int i = 30; i < 35; i++)
		move_coords[i].x = 40 - i, move_coords[i].y = 6;
	for (int i = 35; i < 39; i++)
		move_coords[i].x = 6, move_coords[i].y = i - 28;
	move_coords[39].x = 5, move_coords[39].y = 10;
	for (int i = 40; i < 44; i++)
		move_coords[i].x = 5, move_coords[i].y = 49 - i;
}

void flip_mat_90_cc(int mat[11][11])
{
	for (int i = 0; i < 11; i++)
		for (int j = i; j < 11; j++)
		{
			int aux = mat[i][j];
			mat[i][j] = mat[j][i];
			mat[j][i] = aux;
		}
	for (int j = 0; j < 5; j++)
		for (int i = 0; i < 11; i++)
		{
			int aux = mat[j][i];
			mat[j][i] = mat[10 - j][i];
			mat[10 - j][i] = aux;
		}
}

int main()
{
	move_coords_setup();

	for (int i = 0; i < 4; i++)
		state[i].moved = 0;

	table = (int(*)[11])mmap(NULL, sizeof(int[11][11]), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	connected = (int(*))mmap(NULL, sizeof(int[4]), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	connected_count = (int(*))mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	disconnected_count = (int(*))mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	turn = (int(*))mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	won = (int(*))mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	*connected_count = 0;
	*disconnected_count = 0;
	*turn = 0;
	*won = 0;
	for (int i = 0; i < 4; i++)
		connected[i] = 0;
	table_setup(table);
	struct sockaddr_in server; // structura folosita de server
	struct sockaddr_in from;   // struct folosit pentru a receptiona messajul de la client
	char msg[100];			   // mesajul primit de la client
	char msgrasp[100] = " ";   // mesaj de raspuns pentru client
	int sd;					   // descriptorul de socket

	sem_unlink("/sync_clients");
	client_sync = sem_open("/sync_clients", O_CREAT | O_EXCL, 0666, 1);

	if (client_sync == SEM_FAILED)
	{
		perror("Eroare la crearea semaforului client_sync");
		return 1;
	}

	/* crearea unui socket */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("[server]Eroare la socket().\n");
		return errno;
	}

	/* pregatirea structurilor de date */
	bzero(&server, sizeof(server));
	bzero(&from, sizeof(from));

	/* umplem structura folosita de server */
	/* stabilirea familiei de socket-uri */
	server.sin_family = AF_INET;
	/* acceptam orice adresa */
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	/* utilizam un port utilizator */
	server.sin_port = htons(PORT);

	/* atasam socketul */
	if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{
		perror("[server]Eroare la bind().\n");
		return errno;
	}

	/* punem serverul sa asculte daca vin clienti sa se conecteze */
	if (listen(sd, 4) == -1)
	{
		perror("[server]Eroare la listen().\n");
		return errno;
	}

	/* servim in mod concurent clientii... */
	while (1)
	{
		int client;
		socklen_t length = sizeof(from);

		if(player_id <= 4)
			printf("[server]Asteptam la portul %d. Momentan sunt %d jucatori conectati.\n", PORT, player_id);
		else printf("[server]Portul este %d, insa serverul a atins numarul maxim de jucator conectatii.\n", PORT);
		fflush(stdout);

		/* acceptam un client (stare blocanta pina la realizarea conexiunii) */
		client = accept(sd, (struct sockaddr *)&from, &length);

		/* eroare la acceptarea conexiunii de la un client */
		if (client < 0)
		{
			perror("[server]Eroare la accept().\n");
			continue;
		}

		if(player_id == 4) player_id++;

		if (player_id < 4)
		{
			connected[player_id++] = 1;
			*connected_count = *connected_count + 1;
		}

		if (player_id > 4)
		{
			printf("[server] Serverul este plin.\n");
			strcpy(msgrasp, "Serverul este plin.");
			write(client, msgrasp, 100);
			close(client);
			continue;
		}
		else
		{
			bzero(msgrasp, 100);
			msgrasp[0] = player_id + '0';
			if (player_id == 4)
				*turn = 1;
			write(client, msgrasp, 100);
		}
		int pid;
		if ((pid = fork()) == -1)
		{
			close(client);
			continue;
		}
		else if (pid > 0)
		{

			// parinte
			close(client);

			while (waitpid(-1, NULL, WNOHANG))
				;
			continue;
		}
		else if (pid == 0)
		{
			// copil
			close(sd);
			int out = 0;
			while (true)
			{

				/*pregatim mesajul de raspuns */
				// bzero(msgrasp,100);
				// strcat(msgrasp,"Hello there fellow summoner.");

				// printf("[server%d]Trimitem mesajul%s\n",player_id, msgrasp);

				// /* returnam mesajul clientului */
				// if (write (client, msgrasp, 100) <= 0)
				// {
				// 	perror ("[server]Eroare la write() catre client.\n");
				// 	continue;		/* continuam sa ascultam */
				// }
				// else
				// 	printf ("[server%d]Mesajul a fost trasmis cu succes.\n", player_id);

				// for reference

				// end reference (in sensul sa am backup la text)

				// start sec critica
				sem_wait(client_sync);
				if (*turn != 0 && !connected[*turn - 1])
				{
					while (!connected[*turn % 4])
						*turn = (*turn + 1) % 4;
					*turn = (*turn % 4) + 1;
				}

				for (int i = 1; i < player_id; i++)
					flip_mat_90_cc(table);

				if (write(client, table, 11 * 11 * sizeof(int)) <= 0)
				{
					printf("[server%d]Eroare la write() catre client.\n", player_id);
					continue; /* continuam sa ascultam */
				}

				for (int i = 0; i < out; i++)
					if (table[move_coords[state[i].moved - 1].y][move_coords[state[i].moved - 1].x] != player_id)
					{
						for (int j = i; j < out - 1; j++)
							state[j].moved = state[j + 1].moved;
						state[out - 1].moved = 0;
						out--;
						i--;
					}

				for (int i = player_id; i <= 4; i++)
					flip_mat_90_cc(table);


				if (write(client, turn, sizeof(int)) <= 0)
				{
					printf("[server%d]Eroare la write() catre client.\n", player_id);
					continue; /* continuam sa ascultam */
				}

				if (*disconnected_count == 3 && *connected_count == 4)
					*won = player_id;

				int ok = 1;
				for (int i = 0; i < 4; i++)
					if (state[i].moved <= 40)
					{
						ok = 0;
						break;
					}

				if (ok == 1)
					*won = player_id;

				char game_state[100];
				strcpy(game_state, "on going");
				if (*won!=0)
					strcpy(game_state, "over");

				if (write(client, game_state, 100) <= 0)
				{
					printf("[server%d]Eroare la write() catre client.\n", player_id);
					continue; /* continuam sa ascultam */
				}

				// dormeo noapte buna sectiunea critica
				sem_post(client_sync);

				if (*won!=0)
				{
					if (write(client, won, sizeof(int)) <= 0)
					{
						printf("[server%d]Eroare la write() catre client.\n", player_id);
						continue; /* continuam sa ascultam */
					}
					printf("Closing client %d due to game being ended.\n", player_id);
					sem_close(client_sync);
					close(client);
					exit(0);
				}


				/* s-a realizat conexiunea, se astepta mesajul */
				bzero(msg, 100);
				//printf("[server%d]Asteptam mesajul...\n", player_id);
				fflush(stdout);

				/* citirea mesajului */
				if (read(client, msg, 100) <= 0)
				{
					perror("[server]Eroare la read() de la client.\n");
					close(client); /* inchidem conexiunea cu clientul */
					continue;	   /* continuam sa ascultam */
				}

				//printf("[server%d]Mesajul de la clientul cu ID-ul %d (turn: %d) a fost receptionat...%s\n", player_id, player_id, *turn, msg);

				if (strcmp(msg, "stop") == 0)
				{
					printf("Closing client %d.\n", player_id);
					// start sec critica
					sem_wait(client_sync);
					*disconnected_count = *disconnected_count + 1;
					connected[player_id - 1] = 0;
					for (int i = 0; i < 11; i++)
						for (int j = 0; j < 11; j++)
							if (table[i][j] == player_id)
								table[i][j] = 0;
					// dormeo noapte buna sectiunea critica
					sem_post(client_sync);
					sem_close(client_sync);
					close(client);
					exit(0);
				}
				if (strcmp(msg, "dice") == 0)
				{
					if (*turn == player_id)
					{
						strcpy(msgrasp, "ok");
						if (write(client, msgrasp, 100) <= 0)
						{
							printf("[server%d]Eroare la write() catre client.\n", player_id);
							continue; /* continuam sa ascultam */
						}
						srand(time(NULL));
						int rolled = rand() % 6 + 1;
						if (write(client, &rolled, sizeof(int)) <= 0)
						{
							printf("[server%d]Eroare la write() catre client.\n", player_id);
							continue; /* continuam sa ascultam */
						}

						char legal_moves[100];
						strcpy(legal_moves, "no");

						// start sec critica
						sem_wait(client_sync);

						for (int i = 1; i < player_id; i++)
							flip_mat_90_cc(table);

						for (int i = 0; i < 4; i++)
						{
							if (state[i].moved == 0 && rolled == 6)
								strcpy(legal_moves, "yes");
							if (state[i].moved != 0 && state[i].moved + rolled <= state[i].max_moved && table[move_coords[state[i].moved + rolled - 1].y][move_coords[state[i].moved + rolled - 1].x] != player_id)
								strcpy(legal_moves, "yes");
						}

						for (int i = player_id; i <= 4; i++)
							flip_mat_90_cc(table);

						// dormeo noapte buna sectiunea critica
						sem_post(client_sync);
						if (write(client, legal_moves, 100) <= 0)
						{
							printf("[server%d]Eroare la write() catre client.\n", player_id);
							continue; /* continuam sa ascultam */
						}

						int temp_x, temp_y, temp_ok = 0;
						if (strcmp(legal_moves, "yes") == 0)
						{

							while (!temp_ok)
							{

								char dice_event_type[100];
								if (read(client, dice_event_type, 100) <= 0)
								{
									printf("[server%d]Eroare la write() catre client.\n", player_id);
									continue; /* continuam sa ascultam */
								}

								if (strcmp(dice_event_type, "stop") == 0)
								{
									printf("Closing client %d.\n", player_id);
									// start sec critica
									sem_wait(client_sync);
									*disconnected_count = *disconnected_count + 1;
									connected[player_id - 1] = 0;
									for (int i = 0; i < 11; i++)
										for (int j = 0; j < 11; j++)
											if (table[i][j] == player_id)
												table[i][j] = 0;
									// dormeo noapte buna sectiunea critica
									sem_post(client_sync);
									sem_close(client_sync);
									close(client);
									exit(0);
								}

								if (read(client, &temp_x, sizeof(int)) <= 0)
								{
									printf("[server%d]Eroare la write() catre client.\n", player_id);
									continue; /* continuam sa ascultam */
								}

								if (read(client, &temp_y, sizeof(int)) <= 0)
								{
									printf("[server%d]Eroare la write() catre client.\n", player_id);
									continue; /* continuam sa ascultam */
								}

								char temp_check[100];
								strcpy(temp_check, "nuhuh");
								// start sec critica
								sem_wait(client_sync);

								for (int i = 1; i < player_id; i++)
									flip_mat_90_cc(table);

								if (table[temp_y][temp_x] == player_id)
								{
									if (temp_x < 2 && temp_y >= 9 && rolled == 6 && table[move_coords[0].y][move_coords[0].x] != player_id)
									{

										switch (table[move_coords[0].y][move_coords[0].x])
										{
										case 1:
											for (int i = player_id; i <= 4; i++)
												flip_mat_90_cc(table);
											if (table[9][0] == 0)
												table[9][0] = 1;
											else if (table[9][1] == 0)
												table[9][1] = 1;
											else if (table[10][0] == 0)
												table[10][0] = 1;
											else
												table[10][1] = 1;
											break;
										case 2:
											for (int i = player_id; i <= 4; i++)
												flip_mat_90_cc(table);
											if (table[0][0] == 0)
												table[0][0] = 2;
											else if (table[0][1] == 0)
												table[0][1] = 2;
											else if (table[1][0] == 0)
												table[1][0] = 2;
											else
												table[1][1] = 2;
											break;
										case 3:
											for (int i = player_id; i <= 4; i++)
												flip_mat_90_cc(table);
											if (table[0][9] == 0)
												table[0][9] = 3;
											else if (table[0][10] == 0)
												table[0][10] = 3;
											else if (table[1][9] == 0)
												table[1][9] = 3;
											else
												table[1][10] = 3;
											break;
										case 4:
											for (int i = player_id; i <= 4; i++)
												flip_mat_90_cc(table);
											if (table[9][9] == 0)
												table[9][9] = 4;
											else if (table[9][10] == 0)
												table[9][10] = 4;
											else if (table[10][9] == 0)
												table[10][9] = 4;
											else
												table[10][10] = 4;
											break;
										default:
											for (int i = player_id; i <= 4; i++)
												flip_mat_90_cc(table);
											break;
										}

										for (int i = 1; i < player_id; i++)
											flip_mat_90_cc(table);

										table[move_coords[0].y][move_coords[0].x] = player_id;
										table[temp_y][temp_x] = 0;
										state[out++].moved = 1;
										temp_ok = 1;
										strcpy(temp_check, "ok");
									}
									else
									{
										for (int i = 0; i < out; i++)
											if (move_coords[state[i].moved - 1].x == temp_x && move_coords[state[i].moved - 1].y == temp_y && state[i].moved + rolled <= state[i].max_moved && table[move_coords[state[i].moved + rolled - 1].y][move_coords[state[i].moved + rolled - 1].x] != player_id)
											{
												if (table[move_coords[state[i].moved + rolled - 1].y][move_coords[state[i].moved + rolled - 1].x] != 0)
												{

													switch (table[move_coords[state[i].moved + rolled - 1].y][move_coords[state[i].moved + rolled - 1].x])
													{
													case 1:
														for (int i = player_id; i <= 4; i++)
															flip_mat_90_cc(table);
														if (table[9][0] == 0)
															table[9][0] = 1;
														else if (table[9][1] == 0)
															table[9][1] = 1;
														else if (table[10][0] == 0)
															table[10][0] = 1;
														else
															table[10][1] = 1;
														break;
													case 2:
														for (int i = player_id; i <= 4; i++)
															flip_mat_90_cc(table);
														if (table[0][0] == 0)
															table[0][0] = 2;
														else if (table[0][1] == 0)
															table[0][1] = 2;
														else if (table[1][0] == 0)
															table[1][0] = 2;
														else
															table[1][1] = 2;
														break;
													case 3:
														for (int i = player_id; i <= 4; i++)
															flip_mat_90_cc(table);
														if (table[0][9] == 0)
															table[0][9] = 3;
														else if (table[0][10] == 0)
															table[0][10] = 3;
														else if (table[1][9] == 0)
															table[1][9] = 3;
														else
															table[1][10] = 3;
														break;
													case 4:
														for (int i = player_id; i <= 4; i++)
															flip_mat_90_cc(table);
														if (table[9][9] == 0)
															table[9][9] = 4;
														else if (table[9][10] == 0)
															table[9][10] = 4;
														else if (table[10][9] == 0)
															table[10][9] = 4;
														else
															table[10][10] = 4;
														break;
													default:
														break;
													}

													for (int i = 1; i < player_id; i++)
														flip_mat_90_cc(table);
												}
												table[temp_y][temp_x] = 0;
												state[i].moved += rolled;
												table[move_coords[state[i].moved - 1].y][move_coords[state[i].moved - 1].x] = player_id;
												temp_ok = 1;
												strcpy(temp_check, "ok");
												break;
											}
									}
								}

								for (int i = player_id; i <= 4; i++)
									flip_mat_90_cc(table);

								// dormeo noapte buna sectiunea critica
								sem_post(client_sync);

								if (write(client, temp_check, 100) <= 0)
								{
									printf("[server%d]Eroare la write() catre client.\n", player_id);
									continue; /* continuam sa ascultam */
								}
							}
						}
						// start sec critica
						sem_wait(client_sync);
						while (!connected[*turn % 4])
							*turn = (*turn + 1) % 4;
						*turn = (*turn % 4) + 1;
						// dormeo noapte buna sectiunea critica
						sem_post(client_sync);
					}
					else
					{
						strcpy(msgrasp, "nuhuh");
						if (write(client, msgrasp, 100) <= 0)
						{
							printf("[server%d]Eroare la write() catre client.\n", player_id);
							continue; /* continuam sa ascultam */
						}
					}
				}

				// temp stuff
				/*
								// start sec critica
								sem_wait(client_sync);

								for (int i = 1; i < player_id; i++)
									flip_mat_90_cc(table);

								for (int i = player_id; i <= 4; i++)
									flip_mat_90_cc(table);

								// dormeo noapte buna sectiunea critica
								sem_post(client_sync);
				*/

			} /* while */
		}	  /* if copil */
	}		  /* while servit */
} 




/* main */

/*

			if (*disconnected_count >= 3)
			{
				printf("[server] server closing (dead end, no connected people)\n.");
				munmap(table, sizeof(int[11][11]));
				munmap(connected, sizeof(int[4]));
				munmap(connected_count, sizeof(int));
				munmap(disconnected_count, sizeof(int));
				munmap(turn, sizeof(int));
				sem_unlink("/sync_clients");
				close(sd);
				return 0;
			}
*/