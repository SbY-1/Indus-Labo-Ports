#include "Common.h"

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
