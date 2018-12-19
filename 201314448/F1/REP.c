#include "REP.h"

void EjecutarREP(char pName[], char pPath[], char pID[])
{
    char Nombre[10] = "\0";
    strcpy(Nombre, pName);
    ConvertirMayusculas(Nombre);

    if(strcmp(Nombre, "MBR") == 0)
    {
        ReporteMBR(pPath, pID);
    }
    else
    {
        if(strcmp(Nombre, "DISK") == 0)
        {
            ReporteDISK(pPath, pID);
        }
        else
        {
            printf("El parametro \"name\" no tiene un valor permitido\n");
        }
    }

}

void ReporteMBR(char pPath[], char pID[])
{
    extern ListaMomunt *LISTADO;
    char PathDisco[100] = "\0";
    int existe = 0;

    struct NodoParticion *aux= LISTADO->primero;

    while(aux != NULL)
    {
        if(aux->ID[2] == pID[2] && aux->ID[3] == pID[3])
        {
            strcpy(PathDisco, aux->Path);
            existe = 1;
            break;
        }
        aux = aux->siguiente;
    }

    if(existe == 1)
    {
        /**Se crea el directorio del reporte**/
        char Directorio[100] = "\0";
        strcpy(Directorio, pPath);
        CrearDirectorio(Directorio);

        char RutaDOT[100] = "\0";
        strcpy(RutaDOT, pPath);
        strcat(RutaDOT, ".dot");

        /**Se crea el .dot y se abre el disco del cual se ba a hacer el reporte**/
        FILE * ArchivoDOT;
        FILE * DISCO;

        ArchivoDOT = fopen(RutaDOT, "w");
        DISCO = fopen(PathDisco, "r+b");

        if(DISCO != NULL && ArchivoDOT != NULL)
        {
            /**Se lee el MBR del disco**/
            struct MBR *MBRDisco = (struct MBR*)malloc(sizeof(struct MBR));
            fseek(DISCO, 0, SEEK_SET);
            fread(MBRDisco, sizeof(MBR), 1, DISCO);


            fprintf(ArchivoDOT, "%s", "digraph MBR{\n");
            fprintf(ArchivoDOT, "%s", "  node [shape=plaintext]\n");



            /**Se crea la tabla del reporte que contiene al MBR**/

            fprintf(ArchivoDOT, "%s", "MBR[label=<\n");
            fprintf(ArchivoDOT, "%s", "<TABLE BORDER=\"2\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n");

            fprintf(ArchivoDOT, "%s", "<TR>\n");
            fprintf(ArchivoDOT, "%s", "<TD COLSPAN=\"24\" BGCOLOR=\"skyblue\" PORT=\"title\">MBR: ");
            fprintf(ArchivoDOT, "%s", PathDisco);
            fprintf(ArchivoDOT, "%s", "</TD>\n");
            fprintf(ArchivoDOT, "%s", "</TR>\n");

            fprintf(ArchivoDOT, "%s", "<TR>\n");
            fprintf(ArchivoDOT, "%s", "<TD COLSPAN=\"12\" PORT=\"mbr_tamaño\">mbr_tamaño</TD>\n");
            fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"mbr_tamaño\">%i</TD>\n", MBRDisco->Tamanio);
            fprintf(ArchivoDOT, "%s", "</TR>\n");

            fprintf(ArchivoDOT, "%s", "<TR>\n");
            fprintf(ArchivoDOT, "%s", "<TD COLSPAN=\"12\" PORT=\"mbr_fecha_creacion\">mbr_fecha_creacion</TD>\n");
            fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"mbr_fecha_creacion\">%s</TD>\n", MBRDisco->FechaCreacion);
            fprintf(ArchivoDOT, "%s", "</TR>\n");

            fprintf(ArchivoDOT, "%s", "<TR>\n");
            fprintf(ArchivoDOT, "%s", "<TD COLSPAN=\"12\" PORT=\"mbr_disk_asignature\">mbr_disk_asignature</TD>\n");
            fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"mbr_disk_asignature\">%i</TD>\n", MBRDisco->disk_asignature);
            fprintf(ArchivoDOT, "%s", "</TR>\n");

            int CooExt = -1;

            for(int i = 0 ; i < 4 ; i++)
            {
                if(MBRDisco->Particiones[i].Status == '1')
                {
                    if(MBRDisco->Particiones[i].Tipo == 'e' || MBRDisco->Particiones[i].Tipo == 'E')
                    {
                        CooExt = i;
                    }

                    fprintf(ArchivoDOT, "%s", "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"PART_STATUS\">PART_STATUS_%i</TD>\n", i);
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"PART_STATUS\">%c</TD>\n", MBRDisco->Particiones[i].Status);
                    fprintf(ArchivoDOT, "%s", "</TR>\n");

                    fprintf(ArchivoDOT, "%s", "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_type\">part_type_%i</TD>\n", i);
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_type\">%c</TD>\n", MBRDisco->Particiones[i].Tipo);
                    fprintf(ArchivoDOT, "%s", "</TR>\n");

                    fprintf(ArchivoDOT, "%s", "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_fit\">part_fit_%i</TD>\n", i);
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_fit\">%c</TD>\n", MBRDisco->Particiones[i].Ajuste);
                    fprintf(ArchivoDOT, "%s", "</TR>\n");

                    fprintf(ArchivoDOT, "%s", "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_start\">part_start_%i</TD>\n", i);
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_start\">%i</TD>\n", MBRDisco->Particiones[i].Inicio);
                    fprintf(ArchivoDOT, "%s", "</TR>\n");

                    fprintf(ArchivoDOT, "%s", "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_size\">part_size_%i</TD>\n", i);
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_size\">%i</TD>\n", MBRDisco->Particiones[i].Tamanio);
                    fprintf(ArchivoDOT, "%s", "</TR>\n");

                    fprintf(ArchivoDOT, "%s", "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_name\">part_name_%i</TD>\n", i);
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_name\">%s</TD>\n", MBRDisco->Particiones[i].Nombre);
                    fprintf(ArchivoDOT, "%s", "</TR>\n");
                }
            }


            fprintf(ArchivoDOT, "%s", "</TABLE>\n");
            fprintf(ArchivoDOT, "%s", ">];\n\n");

            /**Se crean las tablas de todas las pariciones logicas**/

            if(CooExt != -1)
            {
                struct EBR *EBRtmp = (struct EBR*)malloc(sizeof(struct EBR));
                int Indice = MBRDisco->Particiones[CooExt].Inicio;

                fseek(DISCO, MBRDisco->Particiones[CooExt].Inicio, SEEK_SET);
                int ConteoLogicas = 0;

                while(1)
                {
                    fread(EBRtmp, sizeof(EBR), 1, DISCO);
                    if(EBRtmp->Status == '1' || Indice == MBRDisco->Particiones[CooExt].Inicio)
                    {

                        fprintf(ArchivoDOT, "EBR_%i[label=<\n", ConteoLogicas);
                        fprintf(ArchivoDOT, "%s", "<TABLE BORDER=\"2\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n");

                        fprintf(ArchivoDOT, "%s", "<TR>\n");
                        fprintf(ArchivoDOT, "%s", "<TD COLSPAN=\"24\" BGCOLOR=\"#FFFF00\" PORT=\"title\">EBR_");
                        fprintf(ArchivoDOT, "%i", ConteoLogicas);
                        fprintf(ArchivoDOT, "%s", "</TD>\n");
                        fprintf(ArchivoDOT, "%s", "</TR>\n");


                        fprintf(ArchivoDOT, "%s", "<TR>\n");
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"PART_STATUS\">PART_STATUS_%i</TD>\n", ConteoLogicas);
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"PART_STATUS\">%c</TD>\n", EBRtmp->Status);
                        fprintf(ArchivoDOT, "%s", "</TR>\n");

                        fprintf(ArchivoDOT, "%s", "<TR>\n");
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_fit\">part_fit_%i</TD>\n", ConteoLogicas);
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_fit\">%c</TD>\n", EBRtmp->Ajuste);
                        fprintf(ArchivoDOT, "%s", "</TR>\n");

                        fprintf(ArchivoDOT, "%s", "<TR>\n");
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_start\">part_start_%i</TD>\n", ConteoLogicas);
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_start\">%i</TD>\n", EBRtmp->Inicio);
                        fprintf(ArchivoDOT, "%s", "</TR>\n");

                        fprintf(ArchivoDOT, "%s", "<TR>\n");
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_size\">part_size_%i</TD>\n", ConteoLogicas);
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_size\">%i</TD>\n", EBRtmp->Tamanio);
                        fprintf(ArchivoDOT, "%s", "</TR>\n");


                        fprintf(ArchivoDOT, "%s", "<TR>\n");
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_next\">part_next_%i</TD>\n", ConteoLogicas);
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_next\">%i</TD>\n", EBRtmp->Ap_Sig);
                        fprintf(ArchivoDOT, "%s", "</TR>\n");


                        fprintf(ArchivoDOT, "%s", "<TR>\n");
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_name\">part_name_%i</TD>\n", ConteoLogicas);
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"part_name\">%s</TD>\n", EBRtmp->Nombre);
                        fprintf(ArchivoDOT, "%s", "</TR>\n");


                        fprintf(ArchivoDOT, "%s", "</TABLE>\n");
                        fprintf(ArchivoDOT, "%s", ">];\n\n");
                    }
                    else
                    {
                        break;
                    }
                    fseek(DISCO, EBRtmp->Ap_Sig, SEEK_SET);
                    Indice = EBRtmp->Ap_Sig;
                    ConteoLogicas++;
                }

                free(EBRtmp);
            }


            fprintf(ArchivoDOT, "%s", "\n}");

            fclose(DISCO);
            fclose(ArchivoDOT);

            char ComandoDOT [100] = "\0";
            strcpy(ComandoDOT, "dot -Tpdf ");
            strcat(ComandoDOT, RutaDOT);
            strcat(ComandoDOT, " -o ");
            strcat(ComandoDOT, pPath);
            strcat(ComandoDOT, ".pdf");

            system(ComandoDOT);

            free(MBRDisco);

            printf("Se creo correctamente el reporte MBR del disco: %s\n", PathDisco);
        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el reporte MBR.\n", pID);
    }
}

void ReporteDISK(char pPath[], char pID[])
{
    extern ListaMomunt *LISTADO;
    char PathDisco[100] = "\0";
    int existe = 0;

    struct NodoParticion *aux= LISTADO->primero;

    while(aux != NULL)
    {
        if(aux->ID[2] == pID[2] && aux->ID[3] == pID[3])
        {
            strcpy(PathDisco, aux->Path);
            existe = 1;
            break;
        }
        aux = aux->siguiente;
    }

    if(existe == 1)
    {
        /**Se crea el directorio del reporte**/
        char Directorio[100] = "\0";
        strcpy(Directorio, pPath);
        CrearDirectorio(Directorio);

        char RutaDOT[100] = "\0";
        strcpy(RutaDOT, pPath);
        strcat(RutaDOT, ".dot");

        /**Se crea el .dot y se abre el disco del cual se ba a hacer el reporte**/
        FILE * ArchivoDOT;
        FILE * DISCO;

        ArchivoDOT = fopen(RutaDOT, "w");
        DISCO = fopen(PathDisco, "r+b");

        if(DISCO != NULL && ArchivoDOT != NULL)
        {

            /**Se lee el MBR del disco**/
            struct MBR *MBRDisco = (struct MBR*)malloc(sizeof(struct MBR));
            fseek(DISCO, 0, SEEK_SET);
            fread(MBRDisco, sizeof(MBR), 1, DISCO);



            fprintf(ArchivoDOT, "%s", "digraph DISK{\n");
            fprintf(ArchivoDOT, "%s", "  node [shape=record];\n");
            fprintf(ArchivoDOT, "%s", "DISK [label=\"");


            fprintf(ArchivoDOT, "MBR&#92;n%f", ((sizeof(MBR) + 0.0) /MBRDisco->Tamanio)*100);



            int IniciosParticiones[4] = {1,2,3,4};

            for(int i = 0 ; i < 4 ; i++)
            {
                if(MBRDisco->Particiones[i].Status == '1')
                {
                    IniciosParticiones[i] = MBRDisco->Particiones[i].Inicio;
                }
            }

            int Numero1;

            for(int i = 0 ; i < 4 ; i++)
            {
                for(int j = 0 ; j < 4-i; j++)
                {
                    if(IniciosParticiones[j] > IniciosParticiones[j+1])
                    {
                        Numero1 = IniciosParticiones[j+1];

                        IniciosParticiones[j+1] = IniciosParticiones[j];

                        IniciosParticiones[j] = Numero1;
                    }
                }
            }

            /**Ya tenemos los valores de los inicios de las particiones ordenados de menor a mayor**/

            int IndiceActual = sizeof(MBR);
            int TamanioP;
            char Tipo;
            int i = 0;

            while(i < 4)
            {
                if(IniciosParticiones[i] > 10)
                {
                    if(IndiceActual == IniciosParticiones[i])
                    {

                        fprintf(ArchivoDOT, "\n | \n");

                        for(int j = 0 ; j < 4 ; j++)
                        {
                            if(MBRDisco->Particiones[j].Inicio == IniciosParticiones[i])
                            {
                                TamanioP = MBRDisco->Particiones[j].Tamanio;
                                Tipo = MBRDisco->Particiones[j].Tipo;
                            }
                        }

                        if(Tipo == 'p' || Tipo == 'P')
                        {
                            fprintf(ArchivoDOT, "Particion&#92;nPrimaria&#92;n%f", ((TamanioP + 0.0) /MBRDisco->Tamanio)*100);
                        }
                        else
                        {
                            fprintf(ArchivoDOT, "Particion&#92;nExtendida&#92;n%f", ((TamanioP + 0.0) /MBRDisco->Tamanio)*100);
                        }

                        IndiceActual = IndiceActual + TamanioP;

                    }
                    else
                    {
                        fprintf(ArchivoDOT, "\n | \n");
                        fprintf(ArchivoDOT, "Libre&#92;n");
                        if(i < 4)
                        {
                            fprintf(ArchivoDOT, "%f", ((IniciosParticiones[i] - IndiceActual + 0.0) /MBRDisco->Tamanio)*100);
                            IndiceActual = IniciosParticiones[i];
                            i--;
                        }
                        else
                        {
                            fprintf(ArchivoDOT, "%f", ((MBRDisco->Tamanio - IndiceActual + 0.0) /MBRDisco->Tamanio)*100);
                        }

                    }
                }
                i++;
            }

            if(IndiceActual < MBRDisco->Tamanio)
            {
                fprintf(ArchivoDOT, "\n | \n");
                fprintf(ArchivoDOT, "Libre&#92;n");
                fprintf(ArchivoDOT, "%f", ((MBRDisco->Tamanio - IndiceActual + 0.0) /MBRDisco->Tamanio)*100);
            }



            fprintf(ArchivoDOT, "\"];");
            fprintf(ArchivoDOT, "%s", "\n}");
            fclose(DISCO);
            fclose(ArchivoDOT);

            char ComandoDOT [100] = "\0";
            strcpy(ComandoDOT, "dot -Tpng ");
            strcat(ComandoDOT, RutaDOT);
            strcat(ComandoDOT, " -o ");
            strcat(ComandoDOT, pPath);

            system(ComandoDOT);

            printf("Se creo correctamente el reporte DISK del disco: %s\n", PathDisco);
        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el reporte DISK.\n", pID);
    }
}
