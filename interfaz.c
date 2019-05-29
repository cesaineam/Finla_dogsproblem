#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>
#include <ctype.h>
#include <unistd.h>
#define MAXINPUT 256

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <ShellApi.h>
#endif

void limpiar_pantalla()
{
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	system("limpiar_pantalla");
#endif

#if defined(_WIN32) || defined(_WIN64)
	system("cls");
#endif
}

void pausa()
{
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	printf("Presione cualquier tecla para continuar...");
	int c = getchar();
#endif

#if defined(_WIN32) || defined(_WIN64)
	system("pause");
#endif
}

void abiriArchivo(char fileName[256])
{
#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
	char path[256] = "gedit ";
	strcat(path, fileName);
	system(path);
#endif

#if defined(_WIN32) || defined(_WIN64)
	ShellExecute(NULL, NULL, fileName, NULL, NULL, SW_SHOW);
#endif
}
int hash_size = 1999;
int num_registros;
int tablaHash[2000];
int registro_medico = 0;

struct dogType{
	char nombre[32];
	char tipo[32];
	int edad;
	char raza[16];
	int estatura;
	float peso;
	char sexo;
	bool borrado;
	int index;
	int hashAnterior;
	int historiaMedica;
};

void stringMinuscula(char *str)
{
	unsigned char *p = (unsigned char *)str;

	while (*p)
	{
		*p = tolower((unsigned char)*p);
		p++;
	}
}

//funcion para escribir un nuevo registro en el archivo
int escribirregistro(void *ap, int position)
{

	struct dogType *dato;
	dato = ap;
	FILE *f;

	f = fopen("dataDogs.dat", "ab");
	if (f == NULL)
	{
		perror("Error abriendo el archivo dataDogs.dat");
		exit(-1);
	}
	int d = fseek(f, position * sizeof(struct dogType), SEEK_SET);
	if( d == -1){
		printf("error al mover al indice en la funcion escribir registro\n");
	}
	int r = fwrite(dato, sizeof(struct dogType), 1, f);
	if (r == 0)
	{
		perror("Could not write Struct");
		exit(-1);
	}
	fclose(f);
	return 0;
}

//funcion que nos permite buscar una estructura dada su posicion en la tabla
void buscarPorIndice(struct dogType *ap, int index, FILE *f)
{

	struct dogType reg;
	int d = fseek(f, index * sizeof(struct dogType), SEEK_SET);
	if (d == -1)
	{
		printf("error al mover al indice en la funcion: Buscar por indice\n");
	}

	int r = fread(&reg, sizeof(struct dogType), 1, f);
	if (r == 0)
	{
		perror("Error leyendo la estructura");
		exit(-1);
	}

	strcpy(ap->nombre, reg.nombre);
	strcpy(ap->tipo, reg.tipo);
	ap->edad = reg.edad;
	strcpy(ap->raza, reg.raza);
	ap->estatura = reg.estatura;
	ap->peso = reg.peso;
	ap->sexo = reg.sexo;
	ap->borrado = reg.borrado;
	ap->index = reg.index;
	ap->hashAnterior = reg.hashAnterior;
	ap->historiaMedica = reg.historiaMedica;
}

//funcion para contar la cantidad de num_registros
int contarEstructuras(FILE *f)
{
	int r;
	r = fseek(f, 0 * sizeof(struct dogType), SEEK_SET);
	if (r == -1)
	{
		printf("error al mover al index\n");
	}
	struct dogType mascota = {"", "", 0, "", 0, 0.0, 'f', 0, 0};
	int count = 0;
	while (fread(&mascota, sizeof(struct dogType), 1, f) != 0)
		count++;
	return count;
}

//funcion para calcular el has de un string con el m�todo de shift left, shift right
unsigned int funcionHash(const char *word)
{
	unsigned int hash = 0, c;

	for (size_t i = 0; word[i] != '\0'; i++)
	{
		c = (unsigned char)word[i];
		hash = (hash << 3) + (hash >> (sizeof(hash) * CHAR_BIT - 3)) + c;
	}
	return hash % hash_size;
}

//funcion para grabar el hash en disco
void escribirHash()
{
	int status = remove("hash.dat");
	if (status == 0)
	{
		printf("Archivo hash.dat borrado\n");
	}

	FILE *f;
	f = fopen("hash.dat", "ab+");

	if (f == NULL)
	{
		perror("Error abriendo archivo");
		exit(-1);
	}

	int r = fwrite(tablaHash, sizeof(tablaHash), 1, f);

	if (r == 0)
	{
		perror("error escribiendo la estructura");
		exit(-1);
	}
	fclose(f);
}

//funcion para leer el hash desde el archivo
void leerHash()
{

	FILE *f;
	f = fopen("hash.dat", "rb");
	int d = fseek(f, 0 * sizeof(struct dogType), SEEK_SET);
	if (d == -1)
	{
		printf("error al mover al ice deseado\n");
	}

	if (f == NULL)
	{
		perror("error abriendo el archivo hash.dat");
		exit(-1);
	}

	int r = fread(tablaHash, sizeof(tablaHash), 1, f);

	if (r == 0)
	{
		perror("Error leyendo el archivo hash.dat");
		exit(-1);
	}
	fclose(f);
}

int validarOpcionIngresada(char input[MAXINPUT])
{
	int length = strlen(input);
	for (int i = 0; i < length; i++)
	{
		if (!isdigit(input[i]))
		{
			printf("Input no valido\n\n");
			return 0;
			exit(1);
		}
	}
	return 1;
}

int validarEnteroIngresado(char input[MAXINPUT])
{

	int length = strlen(input);
	for (int i = 0; i < length; i++)
	{
		if (!isdigit(input[i]))
		{
			return 0;
			exit(1);
		}
	}
	return 1;
}

int vaildarFloatIngresado(char input[MAXINPUT])
{

	double value;
	char *endptr;
	value = strtod(input, &endptr);
	if ((*endptr == '\0') || (isspace(*endptr) != 0))
		return 1;
	else
		return 0;
}

//funcion para ejecutar el menu
int menuPrincipal()
{

	int opcionMenu;
	printf("\t\t**  Sistema de registro de mascotas **\n\n");
			printf("Ingrese el numero segun la opcion que desee: \n\n");
			printf("**********************************\n\n");
			printf("Opcion 1) Ingresar registro\n");
			printf("Opcion 2) Ver registro\n");
			printf("Opcion 3) Borrar registro\n");
			printf("Opcion 4) Buscar registro\n");
			printf("Opcion 5) Salir\n");
			printf("\n**********************************\n");

	char opcionIngresada[MAXINPUT] = "";
	do
	{
		printf("Opcion: ");
		fgets(opcionIngresada);
	} while (!validarOpcionIngresada(opcionIngresada));

	sscanf(opcionIngresada, "%d", &opcionMenu);
	return opcionMenu;
}

// funcion para validar datos ingresados
int validarValorRegistro(int type, char input[MAXINPUT])
{
	switch (type)
	{
	case 1:
		if (strlen(input) <= 32)
		{
			return 1;
		}
		else
		{
			printf("Nombre muy largo\n\n");
			return 0;
		}
		break;

	case 2:
		if (strlen(input) <= 32)
		{
			return 1;
		}
		else
		{
			printf("Tipo de animal ingresado muy extenso\n\n");
			return 0;
		}
		break;

	case 3:
		if (validarEnteroIngresado(input))
		{
			int value;
			sscanf(input, "%d", &value);
			if (value < 2147483648 && value >= 0)
				return 1;
		}
		else
		{
			printf("edad no valida\n\n");
			return 0;
		}

		break;

	case 4:
		if (strlen(input) <= 16)
		{
			return 1;
		}
		else
		{
			printf("Raza muy larga\n\n");
			return 0;
		}
		break;

	case 5:
		if (validarEnteroIngresado(input))
		{
			int value;
			sscanf(input, "%d", &value);
			if (value < 2147483648 && value >= 0)
				return 1;
		}
		else
		{
			printf("Estatura invalida\n\n");
			return 0;
		}
		break;

	case 6:
		if (vaildarFloatIngresado(input))
		{

			float value = strtof(input, NULL);
			if (value < 3.40282e+38 && value >= 0.0)
				return 1;
		}
		else
		{
			printf("Peso invalido\n\n");
			return 0;
		}
		break;

	case 7:
		if (strlen(input) == 1 && (strcmp(input, "H") == 0 || strcmp(input, "M") == 0))
		{
			return 1;
		}
		else
		{
			printf("Sexo digitado no valido\n\n");
			return 0;
		}
		break;

	default:
		return 1;
		break;
	}
}

//funcion para elimiinar una mascota

int borrarMascota(int cantidadnum_registros, int posicionEstructura)
{
	FILE *f;
	FILE *fp2;
	for (int i = 0; i < 2000; i++)
	{
		tablaHash[i] = -1;
	}

	struct dogType mascota_a_borrar;
	fp2 = fopen("dataDogsCopy.dat", "ab+");

	if (fp2 == NULL)
	{
		perror("Could not open a file");
		exit(-1);
	}

	f = fopen("dataDogs.dat", "ab+");
	if (f == NULL)
	{
		perror("Could not open a file");
		exit(-1);
	}
	//Borrar la historia medica

	struct dogType aux;

	buscarPorIndice(&aux, posicionEstructura, f);

	int iDMedical = aux.historiaMedica;

	char fileNameNumber[256];
	char path[256] = "Historias_clinicas/";
	sprintf(fileNameNumber, "%d", iDMedical);
	strcat(path, fileNameNumber);
	strcat(path, ".txt");
	int yt = fseek(f, 0, SEEK_SET);

	for (int i = 0; i < posicionEstructura; i++)
	{
		int g = fread(&mascota_a_borrar, sizeof(struct dogType), 1, f);
		mascota_a_borrar.index = i;
		int hash = funcionHash(mascota_a_borrar.nombre);
		mascota_a_borrar.hashAnterior = tablaHash[hash];
		tablaHash[hash] = i;
		int gg = fwrite(&mascota_a_borrar, sizeof(struct dogType), 1, fp2);
		int seek = fseek(f, 0, SEEK_CUR);
	}

	if (fp2 == NULL)
	{
		perror("Could not open a file");
		exit(-1);
	}

	if (!(posicionEstructura == (cantidadnum_registros - 1)))
	{

		int seek = fseek(f, ((posicionEstructura + 1) * sizeof(struct dogType)), SEEK_SET);

		for (int j = (posicionEstructura + 1); j < cantidadnum_registros; j++)
		{
			int g = fread(&mascota_a_borrar, sizeof(struct dogType), 1, f);
			mascota_a_borrar.index = (j - 1);
			int hash = funcionHash(mascota_a_borrar.nombre);
			mascota_a_borrar.hashAnterior = tablaHash[hash];
			tablaHash[hash] = (j - 1);
			int gg = fwrite(&mascota_a_borrar, sizeof(struct dogType), 1, fp2);
		}
	}

	fclose(f);
	fclose(fp2);

	limpiar_pantalla();
	if (remove("dataDogs.dat") == 0)
	{
		printf("La informacion de la mascota fue eliminada correctamente\n\n");
		printf("**********************************\n\n");
	}
	else
	{
		printf("Error eliminando la informacion de la mascota\n");
		printf("**********************************\n\n");
	}

	if (remove(path) == 0)
	{
		printf("Historia clinica eliminada con exito\n");
		printf("**********************************\n\n");
	}

	int err = rename("dataDogsCopy.dat", "dataDogs.dat");

	if (err == -1)
	{
		puts("Error al renombrar el archivo");
	}

	escribirHash();
	leerHash();

	num_registros--;
	return 0;
}

void buscar(char name[32], FILE *f)
{
	int hash = funcionHash(name);
	int lastIndex = tablaHash[hash];
	int count = 0;
	struct dogType reg;

	if (lastIndex == -1)
	{
		printf("No hay ninguna mascota en nuestra base de daos con el nombre %s\n", name);
		printf("**********************************\n\n");
	}
	else
	{
		while (lastIndex != -1)
		{
			buscarPorIndice(&reg, lastIndex, f);
			if (strcmp(reg.nombre, name) == 0)
			{
				printf("**********************************\n\n");
				printf("Informacion del registro: %d\n", (reg.index + 1));
				printf("\nNombre: %s\n", reg.nombre);
				printf("Tipo: %s\n", reg.tipo);
				printf("Edad: %d\n", reg.edad);
				printf("Raza: %s\n", reg.raza);
				printf("Estatura: %d\n", reg.estatura);
				printf("Peso: %.2lf\n", reg.peso);
				printf("sexo: %c\n\n", reg.sexo);

				count++;
			}
			lastIndex = reg.hashAnterior;
		}
		if (count != 0)
		{
			printf("**********************************\n");
			printf("existen %d mascotas registradas con el nombre %s\n", count, name);
			printf("**********************************n\n");
		}
		else
		{
			printf("**********************************\n");
			printf("No hay ninguna mascota en nuestra base de daos con el nombre %s\n", name);
			printf("**********************************\n\n");
		}
	}
}
void writeInt(int *write)
{
	FILE *f;
	f = fopen("count.dat", "w+");

	if (f == NULL)
	{
		perror("Could not open file");
		exit(-1);
	}

	int r = fwrite(write, sizeof(write), 1, f);

	if (r == 0)
	{
		perror("Could not write Struct");
		exit(-1);
	}
	fclose(f);
}
int leerEntero()
{
	FILE *f;
	int aux;
	if ((access("count.dat", F_OK) != -1))
	{
		f = fopen("count.dat", "r");
		if (f == NULL)
		{
			perror("No se puede abrir el archivo count.dat");
			exit(-1);
		}
		int r = fread(&aux, sizeof(aux), 1, f);
		if (r == 0)
		{
			perror("error en lectura, funcion leer Entero");
			exit(-1);
		}
		return aux;
	}
	else
	{
		return 0;
	}
}

int main()
{

	limpiar_pantalla();
	printf("\nEspere un momento por favor ...\n");
	printf("**********************************\n");

	registro_medico = leerEntero();
	leerHash();
	FILE *f;
	f = fopen("dataDogs.dat", "rb+");

	if (f == NULL)
	{
		perror("Could not open a file");
		exit(-1);
	}

	num_registros = contarEstructuras(f);
	limpiar_pantalla();

	int menuOption = menuPrincipal();

	while (menuOption != 5)
	{
		limpiar_pantalla();
		switch (menuOption)
		{
		case 1:

			printf("\t\tOpcion 1: Ingresar una mascota\n\n");
			printf("********************************************************\n\n");

			char regInput[MAXINPUT];

			do
			{
				printf("Por favor ingrese el nombre de la mascota (maximo 32 letras): ");
				scanf("%[^\n]%*c", regInput);
			} while (!validarValorRegistro(1, regInput));

			char name[32];
			strcpy(name, regInput);
			stringMinuscula(name);
			printf("**********************************\n");

			do
			{
				printf("Por favor ingrese el tipo de mascota (maximo 32 letras): ");
				scanf("%[^\n]%*c", regInput);
			} while (!validarValorRegistro(2, regInput));

			char type[32];
			strcpy(type, regInput);
			stringMinuscula(type);
			printf("**********************************\n");

			do
			{
				printf("Por favor ingrese la edad (en anios) de la mascota ");
				scanf("%[^\n]%*c", regInput);
			} while (!validarValorRegistro(3, regInput));

			int age;
			sscanf(regInput, "%d", &age);
			printf("**********************************\n");

			do
			{
				printf("Por favor ingrese la raza de la mascota (maximo 16 letras): ");
				scanf("%[^\n]%*c", regInput);
			} while (!validarValorRegistro(4, regInput));

			char breed[16];
			strcpy(breed, regInput);
			stringMinuscula(breed);
			printf("**********************************\n");

			do
			{
				printf("Por favor ingrese la estatura (en centimetros) de la mascota: ");
				scanf("%[^\n]%*c", regInput);
			} while (!validarValorRegistro(5, regInput));

			int height;
			sscanf(regInput, "%d", &height);
			printf("**********************************\n");

			do
			{
				printf("Por favor ingrese el peso (en kilos) de la mascota: ");
				scanf("%[^\n]%*c", regInput);
			} while (!validarValorRegistro(6, regInput));

			float weight = strtof(regInput, NULL);
			printf("**********************************\n");

			do
			{
				printf("Por favor ingrese el sexo de la mascota (H: para hombre o M: para mujer): ");
				scanf("%[^\n]%*c", regInput);
			} while (!validarValorRegistro(7, regInput));
			printf("**********************************\n");

			char sex = regInput[0];

			struct dogType *mascota_nueva = malloc(sizeof(struct dogType));

			strcpy(mascota_nueva->nombre, name);
			strcpy(mascota_nueva->tipo, type);
			mascota_nueva->edad = age;
			strcpy(mascota_nueva->raza, breed);
			mascota_nueva->estatura = height;
			mascota_nueva->peso = weight;
			mascota_nueva->sexo = sex;
			mascota_nueva->borrado = false;
			mascota_nueva->index = num_registros;
			mascota_nueva->historiaMedica = -1;

			int mascota_nuevahash = funcionHash(mascota_nueva->nombre);
			mascota_nueva->hashAnterior = tablaHash[mascota_nuevahash];
			tablaHash[mascota_nuevahash] = mascota_nueva->index;
			pausa();

			escribirregistro(mascota_nueva, mascota_nueva->index);
			num_registros++;
			limpiar_pantalla();
			printf("**********************************\n");
			printf("\nMascota ingresada con exito\n\n");
			printf("**********************************\n\n");
			printf("\t\tDatos de la nueva mascota:\n\n");
			printf("Numero de registro: %d\n", ((mascota_nueva->index) + 1));
			printf("Nombre de la mascota: %s\n", mascota_nueva->nombre);
			printf("Tipo de mascota: %s\n", mascota_nueva->tipo);
			printf("Edad de la mascota: %d\n", mascota_nueva->edad);
			printf("Raza de la mascota: %s\n", mascota_nueva->raza);
			printf("Estatura de la mascota: %d\n", mascota_nueva->estatura);
			printf("Peso de la mascota: %.2lf\n", mascota_nueva->peso);
			printf("Sexo de la mascota: %c\n", mascota_nueva->sexo);

            free(mascota_nueva);
			escribirHash();
			leerHash();
			pausa();
			break;

		case 2:
			fclose(f);
			f = fopen("dataDogs.dat", "rb+");

			if (f == NULL)
			{
				perror("error abriendo archivo");
				exit(-1);
			}
			printf("\t\tOpcion 2: Ingresar una mascota\n");
			printf("**************************************************************\n");

			printf("\nEl sistema actualmente posee %d registros\n\n", num_registros);
			printf("**********************************\n\n");

			char numberInput[MAXINPUT];
			int regNumber;
			do
			{
				printf("Ingrese el numero de registro que desea ver: ");
				gets(numberInput);
				if (!validarEnteroIngresado(numberInput))
				{
					printf("El numero ingresado no es valido\n\n");
				}
				else
				{
					break;
				}
			} while (true);

			sscanf(numberInput, "%d", &regNumber);

			if (regNumber > 0 && regNumber <= num_registros)
			{

				struct dogType searchedreg;
				buscarPorIndice(&searchedreg, (regNumber - 1), f);
				limpiar_pantalla();
				printf("**********************************\n\n");
				printf("Mascota numero %d\n", regNumber);
				printf("\nNombre de la mascota: %s\n", searchedreg.nombre);
				printf("Tipo de mascota: %s\n", searchedreg.tipo);
				printf("Edad de la mascota: %d\n", searchedreg.edad);
				printf("Raza de la mascota: %s\n", searchedreg.raza);
				printf("Estatura de la mascota: %d\n", searchedreg.estatura);
				printf("Peso de la mascota: %.2lf\n", searchedreg.peso);
				printf("Sexo de la mascota: %c\n", searchedreg.sexo);
				printf("Hash de la mascota: %d\n", funcionHash(searchedreg.nombre));
				printf("Identificador medico de la mascota: %d\n\n", searchedreg.historiaMedica);
				printf("**********************************\n");

				char clinicHystoryOption[MAXINPUT] = " ";

				do
				{
					printf("Desea ver la historia clinica (Y/N): ");
					gets(clinicHystoryOption);

					if (strlen(clinicHystoryOption) != 1)
					{
						printf("Comando no valido\n\n");
					}
					else
					{
						if (clinicHystoryOption[0] == 'Y' || clinicHystoryOption[0] == 'y' || clinicHystoryOption[0] == 'N' || clinicHystoryOption[0] == 'n')
						{
							break;
						}
						else
						{
							printf("Comando no valido\n\n");
						}
					}

				} while (true);
				int number = (regNumber - 1);
				if (clinicHystoryOption[0] == 'Y' || clinicHystoryOption[0] == 'y')
				{
					if (searchedreg.historiaMedica == -1)
					{
						regNumber = registro_medico;
						searchedreg.historiaMedica = registro_medico;
						registro_medico++;

						int d = fseek(f, number * sizeof(struct dogType), SEEK_SET);
						if (d == -1)
						{
							printf("error al mover al indice deseado\n");
						}
						int r = fwrite(&searchedreg, sizeof(struct dogType), 1, f);

						if (r == 0)
						{
							perror("error escribiendo estructura");
							exit(-1);
						}
						d = fseek(f, 0 * sizeof(struct dogType), SEEK_SET);
						if (d == -1)
						{
							printf("error al regresar al inicio \n");
						}
						fclose(f);
						f = fopen("dataDogs.dat", "rb+");
						if (f == NULL)
						{
							perror("Error abriendo archivo");
							exit(-1);
						}
					}
					else
					{
						regNumber = searchedreg.historiaMedica;
					}
					char fileName[256] = "HC/";
					char fileNameNumber[256];

					sprintf(fileNameNumber, "%d", regNumber);
					strcat(fileName, fileNameNumber);
					strcat(fileName, ".txt");

					if (!(access(fileName, F_OK) != -1))
					{

						FILE *g = fopen(fileName, "w");

						if (g == NULL)
						{
							printf("Error abriendo archivo!\n");
							exit(1);
						}

						fprintf(g, "**********************************\n");
						fprintf(g, "Informacion de la mascota\n");
						fprintf(g, "\nNombre de la mascota: %s\n", searchedreg.nombre);
						fprintf(g, "Tipo de la mascota: %s\n", searchedreg.tipo);
						fprintf(g, "Edad de la mascota: %d\n", searchedreg.edad);
						fprintf(g, "Raza de la mascota: %s\n", searchedreg.raza);
						fprintf(g, "Estatura de la mascota: %d\n", searchedreg.estatura);
						fprintf(g, "Peso de la mascota: %.2f\n", searchedreg.peso);
						fprintf(g, "Sexo de la mascota: %c\n", searchedreg.sexo);
						fprintf(g, "**********************************\n");
						fprintf(g, "Historia clinica de la mascota: ");

						fclose(g);
					}
					abiriArchivo(fileName);
				}
				limpiar_pantalla();
			}
			else
			{
				limpiar_pantalla();

				printf("\nel registro numero %d no se encuentra en la base de datos del sistema", regNumber);

				printf("**********************************\n");
			}

			pausa();
			break;

		case 3:
			printf("\t\tOpcion 3: Borrar una mascota\n");

			printf("******************************************************\n");
			printf("\nEl sistema actualmente tiene %d registros\n\n", num_registros);

			printf("**********************************\n\n");

			char deleteInput[MAXINPUT];
			int regDeleteNumber;

			do
			{
				printf("Digite el indice del registro que quiere borrar: ");
				gets(deleteInput);
				if (!validarEnteroIngresado(deleteInput))
				{
					printf("Por favor ingrese un numero valido\n\n");
				}
				else
				{
					break;
				}
			} while (true);

			sscanf(deleteInput, "%d", &regDeleteNumber);



			if (regDeleteNumber > 0 && regDeleteNumber <= num_registros)
			{

				limpiar_pantalla();
				printf("**********************************\n");
				printf("Espere un momento por favor ...\n");
				printf("**********************************\n");

				fclose(f);
				borrarMascota(num_registros, (regDeleteNumber - 1));

				f = fopen("dataDogs.dat", "ab+");

				if (f == NULL)
				{
					perror("Error abriendo archivo");
					exit(-1);
				}
			}
			else
			{
				limpiar_pantalla();
				printf("\n El registro numero %d no se encuentra en la base de datos del sistema\n", regDeleteNumber);
				printf("**********************************\n");
			}

			pausa();
			break;

		case 4:
			printf("\t\tOpcion 4: Busqueda de una mascota\n");
			printf("**************************************************************\n\n");
			printf("Ingrese el nombre a busar en los registros (Max 32 caracteres): ");

			leerHash();
			char nameInput[MAXINPUT];

			do
			{
				gets(nameInput);
				if (strlen(nameInput) <= 32)
				{
					break;
				}
				else
				{
					printf("El nombre ingresado no es valido\n\n");
				}
			} while (true);

			limpiar_pantalla();
			stringMinuscula(nameInput);
			buscar(nameInput, f);
			pausa();
			break;

		case 5:
			break;

		default:
			printf("**********************************\n");
			printf("Por favor ingrese una opcion valida\n");

			printf("**********************************\n");

			pausa();
			break;
		}
		limpiar_pantalla();
		menuOption = menuPrincipal();
	}
	writeInt(&registro_medico);
	limpiar_pantalla();
	printf("**********************************\n\n");
	printf("Gracias por usar nuestro programa :)  ...\n");

	printf("**********************************\n");
	fclose(f);
	pausa();

	return 0;
}
