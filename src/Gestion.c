#include "Gestion.h"

int main()
{
	int i;
	int nb_ports = atoi(getProp(PROP_FILE, "nb_ports"));
	int nb_boats = atoi(getProp(PROP_FILE, "nb_boats"));
	char* ports_name[] = {"Dunkerque", "Calais", "Douvre"};
	pid_t child_pid;

	// Création des ressources nécessaires
	// MUTEX_BATEAU

	// SHM_BATEAU

	// Création des bateaux
	for (i = 0; i < nb_boats; i++)
	{
		if ((child_pid = fork()) < 0)
		{
			perror("fork failure");
			exit(1);
		}

		if (child_pid == 0)
		{
			printf("Création d'un bateau\n");
			execl("Boat", "BOAT", NULL);
		}
	}

	// Création des ports
	for (i = 0; i < nb_ports; i++)
	{
		if ((child_pid = fork()) < 0)
		{
			perror("fork failure");
			exit(1);
		}

		if (child_pid == 0)
		{
			printf("Création d'un port\n");
			execl("Port", "PORT", ports_name[i], NULL);
		}
	}

	return EXIT_SUCCESS;
}

char* getProp(const char *fileName, const char *propName)
{
	FILE* 	file = NULL;
	char* 	token = NULL;
	char 	line[128];
	char	sep[2] = "=";
	int 	i;
	int 	loginFound = 0;

	if ((file = fopen(fileName, "r")) == NULL)
	{
		perror("Opening file\n");
		exit(errno);
	}
	else
	{
      	while (fgets(line, sizeof line, file) != NULL)
      	{
   			token = strtok(line, sep);
			i = 0;

  			while(token != NULL) 
   			{
				if (i == 0)
				{
					if (strcmp(token, propName) == 0)
						loginFound++;
				}
				else if (i != 0 && loginFound == 1)
				{
					char *password = malloc(sizeof(char *) * 30);
					strcpy(password, token);
					fclose(file);
					return password;
				}
      			token = strtok(NULL, sep);
				i++;
   			}
     	}
	}

	fclose(file);
	return NULL;
}