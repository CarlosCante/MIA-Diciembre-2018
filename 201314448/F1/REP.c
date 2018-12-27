#include "REP.h"

void EjecutarREP(char pName[], char pPath[], char pID[], char pRuta[])
{
    char Nombre[10] = "\0";
    strcpy(Nombre, pName);
    ConvertirMayusculas(Nombre);

    if(strcmp(Nombre, "MBR") == 0)
    {
        ReporteMBR(pPath, pID);
    }
    else if(strcmp(Nombre, "DISK") == 0)
    {
        ReporteDISK(pPath, pID);
    }
    else if(strcmp(Nombre, "INODE") == 0)
    {
        ReporteINODE(pPath, pID);
    }
    else if(strcmp(Nombre, "BLOCK") == 0)
    {
        ReporteBLOCK(pPath, pID);
    }
    else if(strcmp(Nombre, "BM_INODE") == 0)
    {
        ReporteBM_INODE(pPath, pID);
    }
    else if(strcmp(Nombre, "BM_BLOCK") == 0)
    {
        ReporteBM_BLOCK(pPath, pID);
    }
    else if(strcmp(Nombre, "SB") == 0)
    {
        ReporteSB(pPath, pID);
    }
    else if(strcmp(Nombre, "TREE") == 0)
    {
        ReporteTREE(pPath, pID);
    }
    else if(strcmp(Nombre, "FILE") == 0)
    {
        ReporteFILE(pPath, pID, pRuta);
    }
    else if(strcmp(Nombre, "JOURNALING") == 0)
    {
        ReporteJOURNALING(pPath, pID);
    }
    else
    {
        printf("El parametro \"name\" no tiene un valor permitido\n");
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

            char ComandoDOT [500] = "\0";
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

            char ComandoDOT [500] = "\0";
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

void ReporteINODE(char pPath[], char pID[])
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
            struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
            fseek(DISCO, aux->inicio, SEEK_SET);
            fread(SB, sizeof(SuperBloque), 1, DISCO);

            struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
            memset(InodoTMP, 0, sizeof(Inodo));

            fprintf(ArchivoDOT, "%s", "digraph MBR{\n");
            fprintf(ArchivoDOT, "%s", "  node [shape=plaintext]\n");

            char C;

            for(int i = 0 ; i < SB->Inodos_count ; i++)
            {
                fseek(DISCO, SB->inicio_BM_Inodos + i, SEEK_SET);
                fread(&C, sizeof(char), 1, DISCO);

                if(C == '1')
                {
                    fseek(DISCO, SB->inicio_Inodos + i*sizeof(Inodo), SEEK_SET);
                    fread(InodoTMP, sizeof(Inodo), 1, DISCO);


                    fprintf(ArchivoDOT, "INODO_%i[label=<\n", i);
                    fprintf(ArchivoDOT, "<TABLE BORDER=\"2\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"24\" BGCOLOR=\"#FFFF00\" PORT=\"title\">INODO_");
                    fprintf(ArchivoDOT, "%i", i);
                    fprintf(ArchivoDOT, "</TD>\n");
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_uid\">i_uid</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_uid\">%i</TD>\n", InodoTMP->ID_Usuario);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_gid\">i_gid</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_gid\">%i</TD>\n", InodoTMP->ID_Grupo);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_size\">i_size</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_size\">%i</TD>\n", InodoTMP->Tamanio);
                    fprintf(ArchivoDOT, "</TR>\n");



                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_atime\">i_atime</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_atime\">%s</TD>\n", InodoTMP->fecha_lectura);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_ctime\">i_ctime</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_ctime\">%s</TD>\n", InodoTMP->fecha_creacion);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_mtime\">i_mtime</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_mtime\">%s</TD>\n", InodoTMP->fecha_modificacion);
                    fprintf(ArchivoDOT, "</TR>\n");


                    for(int k = 0 ; k < 15 ; k++)
                    {
                        fprintf(ArchivoDOT, "<TR>\n");
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"APUNTADOR\">APUNTADOR_%i</TD>\n", k);
                        fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"APUNTADOR\">%i</TD>\n", InodoTMP->ap_Bloques[k]);
                        fprintf(ArchivoDOT, "</TR>\n");
                    }

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_type\">i_type</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_type\">%c</TD>\n", InodoTMP->tipo);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_perm\">i_perm</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"i_perm\">%i</TD>\n", InodoTMP->Permisos);
                    fprintf(ArchivoDOT, "</TR>\n");


                    fprintf(ArchivoDOT, "</TABLE>\n");
                    fprintf(ArchivoDOT, ">];\n\n");
                }
            }

            fprintf(ArchivoDOT, "\n}");


            fclose(DISCO);
            fclose(ArchivoDOT);

            free(SB);
            free(InodoTMP);

            char ComandoDOT [500] = "\0";
            strcpy(ComandoDOT, "dot -Tpng ");
            strcat(ComandoDOT, RutaDOT);
            strcat(ComandoDOT, " -o ");
            strcat(ComandoDOT, pPath);

            system(ComandoDOT);

            printf("Se creo correctamente el reporte INODE del disco: %s\n", PathDisco);
        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el reporte INODE.\n", pID);
    }
}

void ReporteBM_INODE(char pPath[], char pID[])
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

        /**Se crea el .dot y se abre el disco del cual se ba a hacer el reporte**/
        FILE * Archivo;
        FILE * DISCO;

        Archivo = fopen(pPath, "w");
        DISCO = fopen(PathDisco, "r+b");

        if(DISCO != NULL && Archivo != NULL)
        {
            struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
            fseek(DISCO, aux->inicio, SEEK_SET);
            fread(SB, sizeof(SuperBloque), 1, DISCO);

            char C;
            int bandera = 0;

            for(int i = 0 ; i < SB->Inodos_count ; i++)
            {
                fseek(DISCO, SB->inicio_BM_Inodos + i, SEEK_SET);
                fread(&C, sizeof(char), 1, DISCO);

                fprintf(Archivo, " %c ", C);
                bandera++;

                if(bandera >= 20)
                {
                    fprintf(Archivo, "\n", C);
                    bandera = 0;
                }
            }


            fclose(DISCO);
            fclose(Archivo);

            free(SB);

            printf("Se creo correctamente el reporte BM_INODE del disco: %s\n", PathDisco);
        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el reporte BM_INODE.\n", pID);
    }
}

void ReporteBM_BLOCK(char pPath[], char pID[])
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

        /**Se crea el .dot y se abre el disco del cual se ba a hacer el reporte**/
        FILE * Archivo;
        FILE * DISCO;

        Archivo = fopen(pPath, "w");
        DISCO = fopen(PathDisco, "r+b");

        if(DISCO != NULL && Archivo != NULL)
        {
            struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
            fseek(DISCO, aux->inicio, SEEK_SET);
            fread(SB, sizeof(SuperBloque), 1, DISCO);

            char C;
            int bandera = 0;

            for(int i = 0 ; i < SB->Bloques_count ; i++)
            {
                fseek(DISCO, SB->inicio_BM_Bloques + i, SEEK_SET);
                fread(&C, sizeof(char), 1, DISCO);

                fprintf(Archivo, " %c ", C);
                bandera++;

                if(bandera >= 20)
                {
                    fprintf(Archivo, "\n", C);
                    bandera = 0;
                }
            }


            fclose(DISCO);
            fclose(Archivo);

            free(SB);

            printf("Se creo correctamente el reporte BM_BLOCK del disco: %s\n", PathDisco);
        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el reporte BM_BLOCK.\n", pID);
    }
}


void ReporteSB(char pPath[], char pID[])
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
            struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
            fseek(DISCO, aux->inicio, SEEK_SET);
            fread(SB, sizeof(SuperBloque), 1, DISCO);

            fprintf(ArchivoDOT, "%s", "digraph SB{\n");
            fprintf(ArchivoDOT, "%s", "  node [shape=plaintext]\n");

                    fprintf(ArchivoDOT, "SUPER_BLOQUE[label=<\n");
                    fprintf(ArchivoDOT, "<TABLE BORDER=\"2\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"24\" BGCOLOR=\"#FFFF00\" PORT=\"title\">SUPER_BLOQUE");
                    fprintf(ArchivoDOT, "</TD>\n");
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_filesystem_type\">s_filesystem_type</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_filesystem_type\">%i</TD>\n", SB->Filesystem_Type);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_inodes_count\">s_inodes_count</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_inodes_count\">%i</TD>\n", SB->Inodos_count);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_blocks_count\">s_blocks_count</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_blocks_count\">%i</TD>\n", SB->Bloques_count);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_free_blocks_count\">s_free_blocks_count</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_free_blocks_count\">%i</TD>\n", SB->free_Bloques_count);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_free_inodes_count\">s_free_inodes_count</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_free_inodes_count\">%i</TD>\n", SB->free_Inodos_count);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_mtime\">s_mtime</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_mtime\">%s</TD>\n", SB->fecha_mount);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_umtime\">s_umtime</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_umtime\">%s</TD>\n", SB->fecha_unmount);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_mnt_count\">s_mnt_count</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_mnt_count\">%i</TD>\n", SB->mount_count);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_magic\">s_magic</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_magic\">%i</TD>\n", SB->magic_number);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_inode_size\">s_inode_size</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_inode_size\">%i</TD>\n", SB->Inodo_size);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_block_size\">s_block_size</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_block_size\">%i</TD>\n", SB->Bloque_size);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_first_ino\">s_first_ino</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_first_ino\">%i</TD>\n", SB->first_free_Inodo);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_first_blo\">s_first_blo</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_first_blo\">%i</TD>\n", SB->first_free_Bloque);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_bm_inode_start\">s_bm_inode_start</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_bm_inode_start\">%i</TD>\n", SB->inicio_BM_Inodos);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_bm_block_start\">s_bm_block_start</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_bm_block_start\">%i</TD>\n", SB->inicio_BM_Bloques);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_inode_start\">s_inode_start</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_inode_start\">%i</TD>\n", SB->inicio_Inodos);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "<TR>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_block_start\">s_block_start</TD>\n");
                    fprintf(ArchivoDOT, "<TD COLSPAN=\"12\" PORT=\"s_block_start\">%i</TD>\n", SB->inicio_Bloques);
                    fprintf(ArchivoDOT, "</TR>\n");

                    fprintf(ArchivoDOT, "</TABLE>\n");
                    fprintf(ArchivoDOT, ">];\n\n");


            fprintf(ArchivoDOT, "\n}");


            fclose(DISCO);
            fclose(ArchivoDOT);

            free(SB);

            char ComandoDOT [500] = "\0";
            strcpy(ComandoDOT, "dot -Tpng ");
            strcat(ComandoDOT, RutaDOT);
            strcat(ComandoDOT, " -o ");
            strcat(ComandoDOT, pPath);

            system(ComandoDOT);

            printf("Se creo correctamente el reporte SB del disco: %s\n", PathDisco);
        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el reporte SB.\n", pID);
    }
}


void ReporteTREE(char pPath[], char pID[])
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
            struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
            fseek(DISCO, aux->inicio, SEEK_SET);
            fread(SB, sizeof(SuperBloque), 1, DISCO);

            struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
            memset(InodoTMP, 0, sizeof(Inodo));

            struct BloqueArchivo *BloqueA_TMP = (struct BloqueArchivo*)malloc(sizeof(struct BloqueArchivo));
            memset(BloqueA_TMP, 0, sizeof(BloqueArchivo));

            struct BloqueCarpeta *BloqueC_TMP = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
            memset(BloqueC_TMP, 0, sizeof(BloqueCarpeta));

            fprintf(ArchivoDOT, "%s", "digraph SB{\n");

            fprintf(ArchivoDOT, "   rankdir=\"LR\";\n");
            fprintf(ArchivoDOT, "   node [shape=record, color=blue];\n");
            fprintf(ArchivoDOT, "   style=\"bold, filled, striped\";\n");

            ApuntarNodosTREE(DISCO, ArchivoDOT, SB, 0);

            fprintf(ArchivoDOT, "\n}");


            fclose(DISCO);
            fclose(ArchivoDOT);

            free(SB);

            char ComandoDOT [500] = "\0";
            strcpy(ComandoDOT, "dot -Tpng ");
            strcat(ComandoDOT, RutaDOT);
            strcat(ComandoDOT, " -o ");
            strcat(ComandoDOT, pPath);

            system(ComandoDOT);

            printf("Se creo correctamente el reporte TREE del disco: %s\n", PathDisco);
        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el reporte TREE.\n", pID);
    }
}

void ApuntarNodosTREE(FILE *DISCO, FILE *DOT, SuperBloque *SB, int AP_Inodo)
{
    struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoTMP, 0, sizeof(Inodo));

    struct BloqueArchivo *BloqueA_TMP = (struct BloqueArchivo*)malloc(sizeof(struct BloqueArchivo));
    memset(BloqueA_TMP, 0, sizeof(BloqueArchivo));

    struct BloqueCarpeta *BloqueC_TMP = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
    memset(BloqueC_TMP, 0, sizeof(BloqueCarpeta));

    fseek(DISCO, SB->inicio_Inodos + AP_Inodo*sizeof(Inodo), SEEK_SET);
    fread(InodoTMP, sizeof(Inodo), 1, DISCO);

    if(InodoTMP->tipo == '0')
    {
        fprintf(DOT, "       node[width=2,style=\"filled\",fillcolor=\"blue\"];\n");
        fprintf(DOT, "       inodo%i[fillcolor=\"red\",label=\"\n", AP_Inodo);

        fprintf(DOT, "              {INODO %i}|\n", AP_Inodo);
        fprintf(DOT, "              {i_uid | %i}|\n", InodoTMP->ID_Usuario);
        fprintf(DOT, "              {i_gid | %i}|\n", InodoTMP->ID_Grupo);
        fprintf(DOT, "              {i_size | %i}|\n", InodoTMP->Tamanio);
        fprintf(DOT, "              {i_atime | %s}|\n", InodoTMP->fecha_lectura);
        fprintf(DOT, "              {i_mtime | %s}|\n", InodoTMP->fecha_modificacion);
        fprintf(DOT, "              {i_ctime | %s}|\n", InodoTMP->fecha_creacion);


        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
        {
                fprintf(DOT, "              {i_block[%i] | <c%i> %i}|\n", i, InodoTMP->ap_Bloques[i], InodoTMP->ap_Bloques[i]);
            }
            else
            {
                fprintf(DOT, "              {i_block[%i] | -1}|\n", i);
            }
        }

        fprintf(DOT, "              {i_type | %c}|\n", InodoTMP->tipo);
        fprintf(DOT, "              {i_perm | %i}|\n", InodoTMP->Permisos);
        fprintf(DOT, "      \"];\n\n");

        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {

                fprintf(DOT, "       node[width=2,style=\"filled\",fillcolor=\"blue\"];\n");
                fprintf(DOT, "       bloque%i[fillcolor=\"white\",label=\"\n", InodoTMP->ap_Bloques[i]);

                fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueCarpeta), SEEK_SET);
                fread(BloqueC_TMP, sizeof(BloqueCarpeta), 1, DISCO);

                fprintf(DOT, "              {BLOQUE CARPETA %i}|\n", InodoTMP->ap_Bloques[i]);

                if(i == 0)
                {
                    fprintf(DOT, "           {. | %i}|\n", BloqueC_TMP->b_content[0].ap_inodo);
                    fprintf(DOT, "           {. . | %i}|\n", BloqueC_TMP->b_content[1].ap_inodo);
                    if(BloqueC_TMP->b_content[2].ap_inodo != -1)
                    {
                        fprintf(DOT, "              {%s | <c%i> %i}|\n", BloqueC_TMP->b_content[2].nombre , BloqueC_TMP->b_content[2].ap_inodo , BloqueC_TMP->b_content[2].ap_inodo);
                    }
                    else
                    {
                        fprintf(DOT, "              {AP 2 | -1}|\n");
                    }

                    if(BloqueC_TMP->b_content[3].ap_inodo != -1)
                    {
                        fprintf(DOT, "              {%s | <c%i> %i}|\n", BloqueC_TMP->b_content[3].nombre , BloqueC_TMP->b_content[3].ap_inodo , BloqueC_TMP->b_content[3].ap_inodo);
                    }
                    else
                    {
                        fprintf(DOT, "              {AP 3 | -1}|\n");
                    }

                }
                else
                {
                    for(int k = 0 ; k < 4 ; k++)
                    {
                        if(BloqueC_TMP->b_content[k].ap_inodo != -1)
                        {
                            fprintf(DOT, "              {%s | <c%i> %i}|\n", BloqueC_TMP->b_content[k].nombre , BloqueC_TMP->b_content[k].ap_inodo , BloqueC_TMP->b_content[k].ap_inodo);
                        }
                        else
                        {
                            fprintf(DOT, "              {AP %i | -1}|\n", k);
                        }
                    }
                }


                fprintf(DOT, "      \"];\n\n");
            }
        }

        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {
                fprintf(DOT, "      inodo%i:c%i -> bloque%i;\n", AP_Inodo, InodoTMP->ap_Bloques[i], InodoTMP->ap_Bloques[i]);
            }
        }

        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {
                fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueCarpeta), SEEK_SET);
                fread(BloqueC_TMP, sizeof(BloqueCarpeta), 1, DISCO);

                for(int k = 0 ; k < 4 ; k++)
                {
                    if(BloqueC_TMP->b_content[k].ap_inodo != -1)
                    {
                        if(i != 0 || (i==0 && k != 0 && k != 1))
                        {
                            ApuntarNodosTREE(DISCO, DOT, SB, BloqueC_TMP->b_content[k].ap_inodo);
                            fprintf(DOT, "  bloque%i:c%i -> inodo%i\n",InodoTMP->ap_Bloques[i], BloqueC_TMP->b_content[k].ap_inodo, BloqueC_TMP->b_content[k].ap_inodo);
                        }
                    }
                }
            }
        }
    }
    else if(InodoTMP->tipo == '1')
    {
        fprintf(DOT, "       node[width=2,style=\"filled\",fillcolor=\"blue\"];\n");
        fprintf(DOT, "       inodo%i[fillcolor=\"red\",label=\"\n", AP_Inodo);

        fprintf(DOT, "              {INODO %i}|\n", AP_Inodo);
        fprintf(DOT, "              {i_uid | %i}|\n", InodoTMP->ID_Usuario);
        fprintf(DOT, "              {i_gid | %i}|\n", InodoTMP->ID_Grupo);
        fprintf(DOT, "              {i_size | %i}|\n", InodoTMP->Tamanio);
        fprintf(DOT, "              {i_atime | %s}|\n", InodoTMP->fecha_lectura);
        fprintf(DOT, "              {i_mtime | %s}|\n", InodoTMP->fecha_modificacion);
        fprintf(DOT, "              {i_ctime | %s}|\n", InodoTMP->fecha_creacion);


        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {
                fprintf(DOT, "              {i_block[%i] | <c%i> %i}|\n", i, InodoTMP->ap_Bloques[i], InodoTMP->ap_Bloques[i]);
            }
            else
            {
                fprintf(DOT, "              {i_block[%i] | -1}|\n", i);
            }
        }

        fprintf(DOT, "              {i_type | %c}|\n", InodoTMP->tipo);
        fprintf(DOT, "              {i_perm | %i}|\n", InodoTMP->Permisos);
        fprintf(DOT, "      \"];\n\n");


        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {

                fprintf(DOT, "       node[width=2,style=\"filled\",fillcolor=\"blue\"];\n");
                fprintf(DOT, "       bloque%i[fillcolor=\"yellow\",label=\"\n", InodoTMP->ap_Bloques[i]);

                fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueArchivo), SEEK_SET);
                fread(BloqueA_TMP, sizeof(BloqueArchivo), 1, DISCO);

                fprintf(DOT, "              {BLOQUE ARCHIVO %i}|\n", InodoTMP->ap_Bloques[i]);
                fprintf(DOT, "              {Contenido | %s}\n", BloqueA_TMP->Contenido);
                fprintf(DOT, "      \"];\n\n");
            }
        }

        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {
                fprintf(DOT, "      inodo%i:c%i -> bloque%i;\n", AP_Inodo, InodoTMP->ap_Bloques[i], InodoTMP->ap_Bloques[i]);
            }
        }
    }




    free(InodoTMP);
    free(BloqueA_TMP);
    free(BloqueC_TMP);
}

void ReporteBLOCK(char pPath[], char pID[])
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
            struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
            fseek(DISCO, aux->inicio, SEEK_SET);
            fread(SB, sizeof(SuperBloque), 1, DISCO);

            struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
            memset(InodoTMP, 0, sizeof(Inodo));

            struct BloqueArchivo *BloqueA_TMP = (struct BloqueArchivo*)malloc(sizeof(struct BloqueArchivo));
            memset(BloqueA_TMP, 0, sizeof(BloqueArchivo));

            struct BloqueCarpeta *BloqueC_TMP = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
            memset(BloqueC_TMP, 0, sizeof(BloqueCarpeta));

            fprintf(ArchivoDOT, "%s", "digraph SB{\n");

            fprintf(ArchivoDOT, "   rankdir=\"LR\";\n");
            fprintf(ArchivoDOT, "   node [shape=record, color=blue];\n");
            fprintf(ArchivoDOT, "   style=\"bold, filled, striped\";\n");

            RecorrerBloques(DISCO, ArchivoDOT, SB, 0);

            fprintf(ArchivoDOT, "\n}");


            fclose(DISCO);
            fclose(ArchivoDOT);

            free(SB);

            char ComandoDOT [500] = "\0";
            strcpy(ComandoDOT, "dot -Tpng ");
            strcat(ComandoDOT, RutaDOT);
            strcat(ComandoDOT, " -o ");
            strcat(ComandoDOT, pPath);

            system(ComandoDOT);

            printf("Se creo correctamente el reporte BLOCK del disco: %s\n", PathDisco);
        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el reporte BLOCK.\n", pID);
    }
}

void RecorrerBloques(FILE *DISCO, FILE *DOT, SuperBloque *SB, int AP_Inodo)
{
    struct Inodo *InodoTMP = (struct Inodo*)malloc(sizeof(struct Inodo));
    memset(InodoTMP, 0, sizeof(Inodo));

    struct BloqueArchivo *BloqueA_TMP = (struct BloqueArchivo*)malloc(sizeof(struct BloqueArchivo));
    memset(BloqueA_TMP, 0, sizeof(BloqueArchivo));

    struct BloqueCarpeta *BloqueC_TMP = (struct BloqueCarpeta*)malloc(sizeof(struct BloqueCarpeta));
    memset(BloqueC_TMP, 0, sizeof(BloqueCarpeta));

    fseek(DISCO, SB->inicio_Inodos + AP_Inodo*sizeof(Inodo), SEEK_SET);
    fread(InodoTMP, sizeof(Inodo), 1, DISCO);

    if(InodoTMP->tipo == '0')
    {

        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {

                fprintf(DOT, "       node[width=2,style=\"filled\",fillcolor=\"blue\"];\n");
                fprintf(DOT, "       bloque%i[fillcolor=\"white\",label=\"\n", InodoTMP->ap_Bloques[i]);

                fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueCarpeta), SEEK_SET);
                fread(BloqueC_TMP, sizeof(BloqueCarpeta), 1, DISCO);

                fprintf(DOT, "              {BLOQUE CARPETA %i}|\n", InodoTMP->ap_Bloques[i]);

                if(i == 0)
                {
                    fprintf(DOT, "           {. | %i}|\n", BloqueC_TMP->b_content[0].ap_inodo);
                    fprintf(DOT, "           {. . | %i}|\n", BloqueC_TMP->b_content[1].ap_inodo);
                    if(BloqueC_TMP->b_content[2].ap_inodo != -1)
                    {
                        fprintf(DOT, "              {%s | <c%i> %i}|\n", BloqueC_TMP->b_content[2].nombre , BloqueC_TMP->b_content[2].ap_inodo , BloqueC_TMP->b_content[2].ap_inodo);
                    }
                    else
                    {
                        fprintf(DOT, "              {AP 2 | -1}|\n");
                    }

                    if(BloqueC_TMP->b_content[3].ap_inodo != -1)
                    {
                        fprintf(DOT, "              {%s | <c%i> %i}|\n", BloqueC_TMP->b_content[3].nombre , BloqueC_TMP->b_content[3].ap_inodo , BloqueC_TMP->b_content[3].ap_inodo);
                    }
                    else
                    {
                        fprintf(DOT, "              {AP 3 | -1}|\n");
                    }

                }
                else
                {
                    for(int k = 0 ; k < 4 ; k++)
                    {
                        if(BloqueC_TMP->b_content[k].ap_inodo != -1)
                        {
                            fprintf(DOT, "              {%s | <c%i> %i}|\n", BloqueC_TMP->b_content[k].nombre , BloqueC_TMP->b_content[k].ap_inodo , BloqueC_TMP->b_content[k].ap_inodo);
                        }
                        else
                        {
                            fprintf(DOT, "              {AP %i | -1}|\n", k);
                        }
                    }
                }
                fprintf(DOT, "      \"];\n\n");
            }
        }


        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {
                fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueCarpeta), SEEK_SET);
                fread(BloqueC_TMP, sizeof(BloqueCarpeta), 1, DISCO);

                for(int k = 0 ; k < 4 ; k++)
                {
                    if(BloqueC_TMP->b_content[k].ap_inodo != -1)
                    {
                        if(i != 0 || (i==0 && k != 0 && k != 1))
                        {
                            RecorrerBloques(DISCO, DOT, SB, BloqueC_TMP->b_content[k].ap_inodo);
                        }
                    }
                }
            }
        }
    }
    else if(InodoTMP->tipo == '1')
    {

        for(int i = 0 ; i < 15 ; i++)
        {
            if(InodoTMP->ap_Bloques[i] != -1)
            {

                fprintf(DOT, "       node[width=2,style=\"filled\",fillcolor=\"blue\"];\n");
                fprintf(DOT, "       bloque%i[fillcolor=\"yellow\",label=\"\n", InodoTMP->ap_Bloques[i]);

                fseek(DISCO, SB->inicio_Bloques + InodoTMP->ap_Bloques[i]*sizeof(BloqueArchivo), SEEK_SET);
                fread(BloqueA_TMP, sizeof(BloqueArchivo), 1, DISCO);

                fprintf(DOT, "              {BLOQUE ARCHIVO %i}|\n", InodoTMP->ap_Bloques[i]);
                fprintf(DOT, "              {Contenido | %s}\n", BloqueA_TMP->Contenido);
                fprintf(DOT, "      \"];\n\n");
            }
        }
    }




    free(InodoTMP);
    free(BloqueA_TMP);
    free(BloqueC_TMP);
}

void ReporteFILE(char pPath[], char pID[], char Ruta[])
{
    EJecutarCAT(Ruta, 1, pPath);
}


void ReporteJOURNALING(char pPath[], char pID[])
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
            struct SuperBloque *SB = (struct SuperBloque*)malloc(sizeof(struct SuperBloque));
            fseek(DISCO, aux->inicio, SEEK_SET);
            fread(SB, sizeof(SuperBloque), 1, DISCO);

            struct Journaling *JO = (struct Journaling*)malloc(sizeof(struct Journaling));
            memset(JO,0, sizeof(struct Journaling));

            fprintf(ArchivoDOT, "digraph JOURNALING{\n");
            fprintf(ArchivoDOT, "   node [shape=plaintext]\n");
            fprintf(ArchivoDOT, "   JOURNALING[label=<\n");
            fprintf(ArchivoDOT, "       <TABLE BORDER=\"2\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"0\">\n");
            fprintf(ArchivoDOT, "           <TR>\n");
            fprintf(ArchivoDOT, "               <TD COLSPAN=\"24\" BGCOLOR=\"#FFFF00\" PORT=\"title\">JOURNALING</TD>\n");
            fprintf(ArchivoDOT, "           </TR>\n");

            fprintf(ArchivoDOT, "           <TR>\n");
            fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Tipo_Operacion\">Tipo_Operacion</TD>\n");
            fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Tipo_Elemento\">Tipo_Elemento</TD>\n");
            fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Nombre\">Nombre</TD>\n");
            fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Fecha\">Fecha</TD>\n");
            fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Propietario\">Propietario</TD>\n");
            fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Permisos\">Permisos</TD>\n");
            fprintf(ArchivoDOT, "           </TR>\n");

            for(int i = 0 ; i < SB->Inodos_count ; i++)
            {
                fseek(DISCO, aux->inicio + sizeof(SuperBloque) + i*sizeof(Journaling), SEEK_SET);
                fread(JO, sizeof(Journaling), 1, DISCO);

                if(JO->Tipo_Operacion != '0')
                {
                    fprintf(ArchivoDOT, "           <TR>\n");
                    if(JO->Tipo_Operacion == '1')
                    {
                        fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Tipo_Operacion\">Creacion de archivo</TD>\n");
                    }
                    else if(JO->Tipo_Operacion == '2')
                    {
                        fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Tipo_Operacion\">Modificacion de archivo</TD>\n");
                    }
                    else if(JO->Tipo_Operacion == '3')
                    {
                        fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Tipo_Operacion\">Creacion de directorio</TD>\n");
                    }

                    if(JO->Tipo_Elemento == '0')
                    {
                        fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Tipo_Elemento\">Carpeta</TD>\n");
                    }
                    else if(JO->Tipo_Elemento == '1')
                    {
                        fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Tipo_Elemento\">Archivo</TD>\n");
                    }

                    fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Nombre\">%s</TD>\n", JO->nombre);
                    fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Fecha\">%s</TD>\n", JO->fecha);
                    fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Propietario\">%s</TD>\n", JO->propietario);
                    fprintf(ArchivoDOT, "               <TD COLSPAN=\"12\" PORT=\"Permisos\">%i</TD>\n", JO->permisos);
                    fprintf(ArchivoDOT, "           </TR>\n");
                }
            }

            fprintf(ArchivoDOT, "       </TABLE>\n");
            fprintf(ArchivoDOT, "   >];\n\n");

            fprintf(ArchivoDOT, "\n}");


            fclose(DISCO);
            fclose(ArchivoDOT);

            free(SB);

            char ComandoDOT [500] = "\0";
            strcpy(ComandoDOT, "dot -Tpdf ");
            strcat(ComandoDOT, RutaDOT);
            strcat(ComandoDOT, " -o ");
            strcat(ComandoDOT, pPath);

            system(ComandoDOT);

            printf("Se creo correctamente el reporte JOURNALING del disco: %s\n", PathDisco);
        }
        else
        {
            printf("EL disco de la particion \"%s\" no existe.\n", pID);
        }
    }
    else
    {
        printf("La particion \"%s\" no esta montada para realizar el reporte JOURNALING.\n", pID);
    }
}

