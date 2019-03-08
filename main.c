#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/**
 * Contine corelatiile fiecarei cifre in imaginea test si indicii tuturor ferestrelor posibile
 */
struct Template
{
    double corelatia;
    unsigned int nrFereastra;

    unsigned int cifra;
    unsigned int ly1;
    unsigned int r1y;
    unsigned int lx1;
    unsigned int r1x;
};

/**
 * Fiind trimisa calea unei imagini se calculeaza padding-ul
 * si se pune intr-un vector continutul imaginii trimise
 * @param cale
 * @return tablou
 */
unsigned int *liniarizareTablou( char *cale )
{
    FILE *fisierlocal = fopen(cale, "rb");

    if (fisierlocal == NULL)
    {
        printf("Fisierul este gol/ Nu a reusit sa fie deschis pentru liniarizarea tabloului");
        return 0;
    }

    fseek(fisierlocal, 18, SEEK_SET);
    unsigned int latime = 0;
    fread(&latime, sizeof(latime), 1, fisierlocal);

    unsigned int inaltime = 0;
    fread(&inaltime, sizeof(inaltime), 1, fisierlocal);

    unsigned int *tablou = calloc(latime * inaltime, sizeof(unsigned int));

    fseek(fisierlocal, 0, SEEK_SET);
    unsigned char *header = calloc(54, sizeof(unsigned char));
    fread(header, 54, 1, fisierlocal);

    //incarc fara rotire...asa cum e ea...linie cu linie
    unsigned int indiceElementeTablou = 1;
    unsigned int i, j;
    unsigned int padding;

    if (latime % 4 != 0)
        padding = 4 - (3 * latime) % 4;
    else
        padding = 0;

    fseek(fisierlocal, 54,SEEK_SET);
    for (i = inaltime; i >= 1; i--)
    {  for (j = 1; j <= latime; j++)
        {
            fread(&tablou[indiceElementeTablou], 3, 1, fisierlocal);
            indiceElementeTablou++;
        }
        fseek(fisierlocal,padding,SEEK_CUR);
    }
    fclose(fisierlocal);

    return tablou;
}

/**
 * Fiind trimisa calea unei imagini se returneaza header-ului ei
 * @param cale
 * @return header
 */
unsigned char *obtineHeader (char *cale)
{
    FILE *fisierlocal = fopen(cale, "rb");

    if (fisierlocal == NULL)
    {
        printf("Fisierul este gol/ Nu a reusit sa fie deschis pentru obtinerea headerului");
        exit(0);
    }

    fseek(fisierlocal, 0, SEEK_SET);
    unsigned char *header = calloc(54, sizeof(unsigned char));
    fread(header, 54 , 1, fisierlocal);

    fclose(fisierlocal);
    return header;
}

/**
 * Salveaza extern o imagine cu ajutorul parametrilor dati
 * @param cale
 * @param tabloulDinMemoriaInterna
 * @param headerCeVine
 */
void salvareInMemorieExterna( char *cale, unsigned int *tabloulDinMemoriaInterna, unsigned char *headerCeVine)
{
    FILE *fisierlocal = fopen(cale, "wb");

    if (fisierlocal == NULL) {
        printf("Fisierul este gol/ Nu a reusit sa fie deschis pentru salvare in memoria externa");
        exit(0);
    }
    unsigned int x = headerCeVine[18];
    unsigned int a, b, c;
    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;
    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    //copii octetii si pun intr-un unsigned int

    unsigned int latime = x;
    unsigned int inaltime = y;

    unsigned int dimensiuneTabloulLiniarizat = latime * inaltime ;

    fwrite(headerCeVine, 1, 54, fisierlocal);

    unsigned int padding = 0;

    if (latime % 4 != 0)
        padding = 4 - (3 * latime) % 4;
    else
        padding = 0;

    unsigned int i = 1, j = 1;
    for( i = 1; i <= dimensiuneTabloulLiniarizat;i++)
    {
        fwrite(&tabloulDinMemoriaInterna[i], 3, 1, fisierlocal);
        if (i % latime == 0)
            fseek(fisierlocal,padding,SEEK_CUR);
    }

    fclose(fisierlocal);
}

/**
 * Functia xor
 * @param n
 * @param seed
 * @return tablou
 */
unsigned int *xorShift32(unsigned int n, unsigned int seed)
{
    unsigned int  r, k;
    unsigned int *tablou = calloc(n + 1, sizeof(int));

    r = seed;

    for( k = 1; k <= n; k++)
    {
        r = r ^ r << 13;
        r = r ^ r >> 17;
        r = r ^ r << 5;
        tablou[k] = r;
    }

    return tablou;
}

/**
 * Genereaza o permutare aleatoare
 * @param n
 * @param tabloulGeneratXor
 * @return tablou
 */
unsigned int *permutare(unsigned int n, unsigned int *tabloulGeneratXor)
{
    unsigned int k = 1, r = 1, aux = 1;
    unsigned int *tablou = calloc( n + 1, sizeof(unsigned int));

    for( k = 1; k <= n; k++)
        tablou[k] = k;

    unsigned int indice = 1;

    for( k = n; k >= 1; k--, indice++ )
    {
        r = tabloulGeneratXor[indice] % k +1;
        aux = tablou[r];
        tablou[r] = tablou[k];
        tablou[k] = aux;
    }

    return tablou;
}

/**
 * Cripteaza o imagine
 * @param caleImagineInitiala
 * @param caleImagineCriptata
 * @param caleCheiaSecreta
 */
void criptareImagine( char *caleImagineInitiala, char *caleImagineCriptata, char *caleCheiaSecreta)
{
    FILE* fisierImgInitiala = fopen(caleImagineInitiala, "rb");

    if (fisierImgInitiala == NULL) {
        printf("Fisierul este gol/ Nu a reusit sa fie deschis pentru criptareImagine");
        exit(0);
    }

    unsigned char *headerCeVine = obtineHeader(caleImagineInitiala);

    unsigned int x = headerCeVine[18];
    unsigned int a, b, c;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    unsigned int dimensiuneTablou = x * y;

    unsigned int *tablouLiniarizat = liniarizareTablou(caleImagineInitiala);

    unsigned int obtineCheiaSecreta1 = 0;
    unsigned int obtineCheiaSecreta2 = 0;

    FILE* fisierCuCheiaSecreta = fopen(caleCheiaSecreta, "r");

    fscanf(fisierCuCheiaSecreta, "%u",&obtineCheiaSecreta1);

    fscanf(fisierCuCheiaSecreta, "%u",&obtineCheiaSecreta2);

    fclose(fisierCuCheiaSecreta);

    unsigned int *genereazaTablouXor2X = NULL;
    genereazaTablouXor2X = xorShift32(dimensiuneTablou * 2 , obtineCheiaSecreta1);

    unsigned int *genereazaTablouPermutare = NULL;
    genereazaTablouPermutare = permutare(dimensiuneTablou, genereazaTablouXor2X);

    unsigned int i;
    unsigned int *tabloulPermutat = calloc(dimensiuneTablou, sizeof(unsigned int));

    for( i = 1; i <= dimensiuneTablou; i = i + 1)
    {
        tabloulPermutat[i] = tablouLiniarizat[genereazaTablouPermutare[i]];
    }

    salvareInMemorieExterna(caleImagineCriptata, tabloulPermutat, headerCeVine);

    unsigned int *tabloulPermutatInvers = calloc(dimensiuneTablou, sizeof(unsigned int));

    for( i = 1; i<= dimensiuneTablou; i++)
        tabloulPermutatInvers[genereazaTablouPermutare[i]] = tablouLiniarizat[i];

    for (i = 1; i <=dimensiuneTablou; i++)
        if( i == 1)
            tabloulPermutat[i] = obtineCheiaSecreta2 ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + 1];
        else
            tabloulPermutat[i] = tabloulPermutat[i-1] ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + i];

    salvareInMemorieExterna(caleImagineCriptata, tabloulPermutat, headerCeVine);

    fclose(fisierCuCheiaSecreta);
    fclose(fisierImgInitiala);
}

/**
 * Decripteaza o imagine
 * @param caleImagineInitiala
 * @param caleImagineCriptata
 * @param caleImagineDecriptata
 * @param caleCheiaSecreta
 */
void decriptareImagine( char *caleImagineInitiala, char *caleImagineCriptata, char *caleImagineDecriptata,char *caleCheiaSecreta )
{

    FILE* fisierImgInitiala = fopen(caleImagineInitiala, "rb");

    if (fisierImgInitiala == NULL) {
        printf("Fisierul este gol/ Nu a reusit sa fie deschis pentru criptareImagine");
        exit(0);
    }

    unsigned char *headerCeVine = obtineHeader(caleImagineInitiala);

    unsigned int x = headerCeVine[18];
    unsigned int a, b, c, i;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    unsigned int dimensiuneTablou = x * y;

    unsigned int *tablouLiniarizat = liniarizareTablou(caleImagineInitiala);

    unsigned int obtineCheiaSecreta1 = 0;
    unsigned int obtineCheiaSecreta2 = 0;

    FILE* fisierCuCheiaSecreta = fopen(caleCheiaSecreta, "r");

    fscanf(fisierCuCheiaSecreta, "%u",&obtineCheiaSecreta1);

    fscanf(fisierCuCheiaSecreta, "%u",&obtineCheiaSecreta2);

    fclose(fisierCuCheiaSecreta);

    unsigned int *genereazaTablouXor2X = NULL;
    genereazaTablouXor2X = xorShift32(dimensiuneTablou * 2 , obtineCheiaSecreta1);

    unsigned int *genereazaTablouPermutare = NULL;
    genereazaTablouPermutare = permutare(dimensiuneTablou, genereazaTablouXor2X);

    unsigned int *tabloulPermutat = calloc(dimensiuneTablou, sizeof(unsigned int));
    for( i = 1; i <= dimensiuneTablou; i++)
        tabloulPermutat[i] = tablouLiniarizat[genereazaTablouPermutare[i]];

    for (i = 1; i <=dimensiuneTablou; i++)
        if( i == 1)
            tabloulPermutat[i] = obtineCheiaSecreta2 ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + 1];
        else
            tabloulPermutat[i] = tabloulPermutat[i-1] ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + i];

    //criptat

    unsigned int *copieTablouPermutat = calloc(dimensiuneTablou, sizeof(unsigned int));
    for( i = 1; i <= dimensiuneTablou; i++)
        copieTablouPermutat[i] = tabloulPermutat[i];

    unsigned int *tabloulCPrim = calloc(dimensiuneTablou, sizeof(unsigned int));
    for (i = 1; i <=dimensiuneTablou; i++)
        if( i == 1)
            tabloulCPrim[i] = obtineCheiaSecreta2 ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + 1];
        else
            tabloulCPrim[i] = tabloulPermutat[i-1] ^ tabloulPermutat[i] ^ genereazaTablouXor2X[dimensiuneTablou + i];

    for( i = 1; i <= dimensiuneTablou; i++)
        copieTablouPermutat[genereazaTablouPermutare[i]] = tabloulCPrim[i];

    salvareInMemorieExterna(caleImagineDecriptata,copieTablouPermutat, headerCeVine);

    fclose(fisierCuCheiaSecreta);
    fclose(fisierImgInitiala);
}

/**
 * Afiseaza valorile testului Chi Patrat
 * @param caleImagineInitiala
 */
void testulChiPatrat( char *caleImagineInitiala )
{
    unsigned char *headerCeVine = obtineHeader(caleImagineInitiala);

    unsigned int x = headerCeVine[18];
    unsigned int a, b, c, i;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    unsigned int dimensiuneTablou = x * y;
    unsigned int *tablouLiniarizat = liniarizareTablou(caleImagineInitiala);

    unsigned int  j = 0, k = 1;
    double sumaRed = 0, sumaGreen = 0, sumaBlue = 0 , fBaraSus = (double)dimensiuneTablou/256;
    unsigned char byteRed = 0, byteGreen = 0, byteBlue = 0;

    // blue
    for( i = 0; i <= 255; i++)
    {
        unsigned int nr = 0;
        for( k = 1; k <= dimensiuneTablou; k++)
        {
            byteBlue = tablouLiniarizat[k];
            if(byteBlue == i)
                nr ++;
        }
        sumaBlue = sumaBlue + (( nr - fBaraSus ) * ( nr - fBaraSus ) ) / fBaraSus;
    }

    // green
    for( i = 0; i <= 255; i++)
    {
        unsigned int nr = 0;
        for( k = 1; k <= dimensiuneTablou; k++)
        {
            byteGreen = tablouLiniarizat[k] >>8;
            if(byteGreen == i)
                nr ++;
        }
        sumaGreen = sumaGreen + (( nr - fBaraSus ) * ( nr - fBaraSus ) ) / fBaraSus;
    }

    // red
    for( i = 0; i <= 255; i++)
    {
        unsigned int nr = 0;
        for( k = 1; k <= dimensiuneTablou; k++)
        {
            byteRed = tablouLiniarizat[k] >>16;
            if(byteRed == i)
                nr ++;
        }
        sumaRed = sumaRed + (( nr - fBaraSus ) * ( nr - fBaraSus ) ) / fBaraSus;
    }

    printf(" albastru %0.2f \n verde %0.2f \n rosu %0.2f", sumaBlue, sumaGreen, sumaRed);
}

/**
 * Returneaza un tablou inversat
 * @param tabloulLiniarizatCeVine
 * @param caleImagineInitiala
 * @return tablou
 */
unsigned int *inversare(unsigned int *tabloulLiniarizatCeVine, char *caleImagineInitiala)
{
    unsigned char *headerCeVine = obtineHeader(caleImagineInitiala);

    unsigned int x = headerCeVine[18];
    unsigned int a, b, c, i;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    unsigned int inaltime = y;
    unsigned int latime = x;

    unsigned int aux = 0, j;

    for( i = 1; i<= inaltime/2; i++)
        for(j = 1; j<= latime; j++)
        {
            aux = tabloulLiniarizatCeVine[i * latime + j];
            tabloulLiniarizatCeVine[i * latime + j] = tabloulLiniarizatCeVine[(inaltime - i + 1) * latime + j];
            tabloulLiniarizatCeVine[(inaltime - i + 1) * latime + j] = aux;
        }

    return tabloulLiniarizatCeVine;
}

/**
 * Deinverseaza elementele unui tablou
 * @param caleImagineInversata
 * @param tablouInversatLiniarizat
 * @return tablou
 */
unsigned int *deInversare(char *caleImagineInversata, unsigned int *tablouInversatLiniarizat)
{
    unsigned char *headerCeVine = obtineHeader(caleImagineInversata);

    unsigned int x = headerCeVine[18];
    unsigned int a, b, c, i;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c;

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f;

    unsigned int inaltime = y;
    unsigned int latime = x;

    unsigned int aux   = 0, j;

    for( i = 1; i<= inaltime/2; i++)
        for(j = 1; j<= latime; j++)
        {
            aux = tablouInversatLiniarizat[i * latime + j];
            tablouInversatLiniarizat[i * latime + j] = tablouInversatLiniarizat[(inaltime - i + 1) * latime + j];
            tablouInversatLiniarizat[(inaltime - i + 1) * latime + j] = aux;

        }

    return tablouInversatLiniarizat;
}


/*
 *
 * -------------------------------------------PARTEA A DOUA A PROIECTULUI---------------------------------------------------------
 *
 */


/**
 * Returneaza calculul corelatiei intr-un tablou
 * si numarul de elemente al acestuia
 * @param caleImagine
 * @param caleSablon
 * @param caleImagineaGrayscale
 * @param cifraSablonului
 * @param indiceTS
 * @return tablou
 */
struct Template *calcululCorelatiei( char *caleImagine, char *caleSablon, char *caleImagineaGrayscale,
                                              unsigned int cifraSablonului, unsigned int *indiceTS)
{
    FILE *fisierImagine = fopen(caleImagine, "rb");
    FILE *fisierSablon  = fopen(caleSablon, "rb");

    if (fisierImagine == NULL)
    {
        printf("fisierImagine este gol sau nu a putut fi deschis");
        return 0;
    }

    if (fisierSablon == NULL)
    {
        printf("fisierSablon este gol sau nu a putut fi deschis");
        return 0;
    }

    //----------------------------------------luam latime si inaltime sablon --------------------

    fseek(fisierSablon, 18, SEEK_SET);
    unsigned int latimeSablon = 0;
    fread(&latimeSablon, sizeof(latimeSablon), 1, fisierSablon);

    unsigned int inaltimeSablon = 0;
    fread(&inaltimeSablon, sizeof(inaltimeSablon), 1, fisierSablon);

    fseek(fisierSablon, 0, SEEK_SET);

    unsigned char *headerSablon = calloc(54, sizeof(unsigned char));
    fread(headerSablon, 54, 1, fisierSablon);

    fseek(fisierSablon, 54, SEEK_SET);

    //--------------------------------- Am luat inaltimea si latimea sablonului ---------------------


    //----------------------------------------------Declarare tip de date sablon--------------------

    double sigmaS = 0;
    double sumaIntermediara = 0;
    double sBaraSus = 0;
    unsigned int n = latimeSablon*inaltimeSablon; //numarul pixelilor
    unsigned int i = 1, j = 1;
    unsigned char baitRed = 0, baitGreen = 0, baitBlue = 0;

    //----------------------------------------------Declarare tip de date imagine-------------------

    double fBaraSus = 0;
    double sigmaFDeI = 0;
    double sumaIntermediaraCorelatia = 0;
    double corelatia = 0;
    unsigned char *headerCeVine = obtineHeader(caleImagine);
    unsigned char aux = 0;
    unsigned int x = headerCeVine[18];
    unsigned int y = headerCeVine[22];
    unsigned int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;

    double sumaFinalaCorelatie = 0;

    //-------------------------------------Citim sablonul intr-o matrice-----------------------------------------

    unsigned int **matriceSablon = calloc(latimeSablon * inaltimeSablon, sizeof(unsigned int *));

    for (i = 1; i <= inaltimeSablon; i++)
        matriceSablon[i] = calloc(latimeSablon, sizeof(unsigned int));

    unsigned int padding = 0;

    if (latimeSablon % 4 != 0)
        padding = 4 - (3 * latimeSablon) % 4;
    else
        padding = 0;

    fseek(fisierSablon, 54,SEEK_SET);

    for (i = 1; i <= inaltimeSablon; i++)
    {
        for (j = 1; j <= latimeSablon; j++)
        {
            fread(&matriceSablon[i][j], 3, 1, fisierSablon);
        }

        fseek(fisierSablon,padding,SEEK_CUR);
    }
        //------------------------------------------------------------facem sablonul grayscale------------------------

    for (i = 1; i <= inaltimeSablon; i++)
        for (j = 1; j <= latimeSablon; j++)
        {
            baitBlue  = (unsigned char) matriceSablon[i][j]       ;
            baitGreen = (unsigned char) (matriceSablon[i][j] >> 8);
            baitRed   = (unsigned char) (matriceSablon[i][j] >> 16);
            aux = (unsigned char) (0.299 * baitRed + 0.587 * baitBlue + 0.114 * baitGreen);
            matriceSablon[i][j] = (aux) + (aux << 8) + (aux << 16);
        }

    //-------------------------------------------------Am facut sablonul  grayscale----------------------

    //-------------------------------------------------Calculam sbarasus pentru sablon-------------------------

    for (i = 1; i <= inaltimeSablon; i++)
        for (j = 1; j <= latimeSablon; j++)
        {
            baitBlue = matriceSablon[i][j];
            sBaraSus += baitBlue;
        }

    sBaraSus = sBaraSus/(double)n;

    //-------------------------------------------------------Am calculat sBaraSuS-------------------------

    for (i = 1; i <= inaltimeSablon; i++)
    {
        for (j = 1; j <= latimeSablon; j++)
        {
            baitBlue = matriceSablon[i][j];
            sumaIntermediara += ((baitBlue - sBaraSus) * (baitBlue - sBaraSus));
        }
    }

    sigmaS = sqrt(sumaIntermediara/ (double) (n - 1));

    // --------------------------------------------------- Avem sigmaS ------------------------------------

   // --------------------------------------------------- Luam latime si inaltime imagine ----------------


    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c; //latime

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f; //inaltime

    fseek(fisierImagine, 54, SEEK_SET);

    unsigned int dimensiuneTablou = x * y;
    unsigned int latime = x, inaltime = y;

    //--------------------------------------- Avem latime si inaltime imagine -------------------------

    // -------------------------------------- Calculam sigmaFDeI -------------------------

    unsigned int **matriceImagine = calloc(latime * inaltime, sizeof(unsigned int *));
    unsigned int indice = 1;

    for (i = 1; i <= inaltime; i++)
        matriceImagine[i] = calloc(latime, sizeof(unsigned int));

    if (latime % 4 != 0)
        padding = 4 - (3 * latime) % 4;
    else
        padding = 0;

    for (i = 1; i <= inaltime; i++)
    {
        for (j = 1; j <= latime; j++)
        {
            fread(&matriceImagine[i][j], 3, 1, fisierImagine);
        }
        fseek(fisierImagine,padding,SEEK_CUR);
    }

    //--------------------------------------------- Facem imaginea grayscale ----------------------

    for (i = 1; i <= inaltime; i++)
        for (j = 1; j <= latime; j++)
        {
            baitBlue  = (unsigned char) matriceImagine[i][j]       ;
            baitGreen = (unsigned char) (matriceImagine[i][j] >> 8);
            baitRed   = (unsigned char) (matriceImagine[i][j] >> 16);
            aux = (unsigned char) (0.299 * baitRed + 0.587 * baitBlue + 0.114 * baitGreen);
            matriceImagine[i][j] = (aux) + (aux << 8) + (aux << 16);
        }

    // ---------------------------------------------Am facut imaginea grayscale----------------------

    unsigned int *tablouImagineCeSeFaceGri = calloc(latime * inaltime, sizeof(unsigned int));
    unsigned contor = 1;

    for (i = 1; i <= inaltime; i++)
        for (j = 1; j <= latime; j++)
        {
            tablouImagineCeSeFaceGri[contor] = matriceImagine[i][j];
            contor++;
        }

    salvareInMemorieExterna(caleImagineaGrayscale, tablouImagineCeSeFaceGri, headerCeVine);

    //-----------------------------------------------Am salvat imaginea grayscale extern-------------

    struct Template *tablouStructura = calloc(latime*inaltime, sizeof(struct Template));

    unsigned int indiceSablonI = 1, indiceSablonJ = 1;
    unsigned int i2 = 1, j2 = 1;

    for (i = 1; i <= inaltime - inaltimeSablon ; i++)
    {
        for (j = 1; j <= latime - latimeSablon ; j++)
        {

         fBaraSus = 0;
         sumaIntermediaraCorelatia = 0;
         sumaFinalaCorelatie = 0;

            for (i2 = i; i2 <= inaltimeSablon + i - 1; i2++)
                for (j2 = j; j2 <= latimeSablon + j - 1; j2++)
                {
                    baitBlue  = matriceImagine[i2][j2];
                    fBaraSus += baitBlue ;
                }

            fBaraSus = fBaraSus / (double) n;

            for (i2 = i; i2 <= inaltimeSablon + i - 1; i2++)
                for (j2 = j; j2 <= latimeSablon + j - 1; j2++)
                {
                    baitBlue = matriceImagine[i2][j2];
                    sumaIntermediaraCorelatia += ( (baitBlue - fBaraSus) * (baitBlue - fBaraSus));
                }

            sigmaFDeI = sqrt(sumaIntermediaraCorelatia / (double) (n - 1));

            //---------------------------------------avem sigmaFDeI -------------------

            //---------------------------------------continuam cu calculul corelatiei--

            for( i2 = i, indiceSablonI = 1; i2 <= inaltimeSablon + i - 1; i2++, indiceSablonI++)
                for (j2 = j, indiceSablonJ = 1; j2 <= latimeSablon + j - 1; j2++, indiceSablonJ++)
                {
                    baitBlue  = matriceImagine[i2][j2];
                    baitGreen = matriceSablon [indiceSablonI][indiceSablonJ];

                    sumaFinalaCorelatie += ((baitBlue - fBaraSus) * (baitGreen - sBaraSus))/(double)(sigmaFDeI * sigmaS);

                }

            corelatia = sumaFinalaCorelatie / (double) n;

            //---------------------------------------------- am calculat corelatia si acum o punem in tablou ---------------

            tablouStructura[*indiceTS].corelatia = corelatia;

            tablouStructura[*indiceTS].lx1 = j;
            tablouStructura[*indiceTS].ly1 = i;
            tablouStructura[*indiceTS].r1x = latimeSablon + j - 1;
            tablouStructura[*indiceTS].r1y = inaltimeSablon + i - 1;

            tablouStructura[*indiceTS].cifra = cifraSablonului;

            (*indiceTS)++;

            //---------------------------------------------- indicele returneaza in parametru, la final, numarul tuturor ferestrelor ---------
        }
    }

    fclose(fisierImagine);
    fclose(fisierSablon);

    return tablouStructura;
}

/**
 * Coloreaza ramele unor ferestre cu o corelatie >0.5
 * @param caleImagine
 * @param culoare
 * @param tablouTemplate
 * @param caleSablon
 * @param ps
 * @param tablou
 * @param cifra
 */
void colorareFereastra (char *caleImagine,
                        unsigned int culoare,
                        struct Template *tablouTemplate,
                        char *caleSablon, double ps, struct Template *tablou, unsigned int cifra)
{
    FILE *fisierImagine = fopen(caleImagine, "rb");
    FILE *fisierSablon = fopen(caleSablon, "rb");

    if (fisierImagine == NULL) {
        printf("fisierImagine este gol sau nu a putut fi deschis");
        return ;
    }

    if (fisierSablon == NULL) {
        printf("fisierSablon este gol sau nu a putut fi deschis");
        return ;
    }

    fseek(fisierSablon, 18, SEEK_SET);
    unsigned int latimeSablon = 0;
    fread(&latimeSablon, sizeof(latimeSablon), 1, fisierSablon);

    unsigned int inaltimeSablon = 0;
    fread(&inaltimeSablon, sizeof(inaltimeSablon), 1, fisierSablon);

    fseek(fisierSablon, 0, SEEK_SET);

    unsigned char *headerSablon = calloc(54, sizeof(unsigned char));
    fread(headerSablon, 54, 1, fisierSablon);

    unsigned int dimSablon = latimeSablon * inaltimeSablon;

    //-------------------------------------------------------avem latime si inaltime sablon-----------------------

    unsigned char *headerCeVine = obtineHeader(caleImagine);
    unsigned int a, b, c;

    unsigned int x = headerCeVine[18];

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c; //latime

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f; //inaltime

    fseek(fisierImagine, 54, SEEK_SET);

    unsigned int latime = x, inaltime = y;

    // ---------------------------------------------------avem latime si inaltime imagine--------------------------

    unsigned int i = 1, j = 1, iTI = 1;

    unsigned int **matriceImagine = calloc(latime * inaltime, sizeof(unsigned int *));

    for (i = 1; i <= inaltime; i++)
        matriceImagine[i] = calloc(latime, sizeof(unsigned int));

    for (i = 1; i <= inaltime; i++)
        for (j = 1; j <= latime; j++)
            fread(&matriceImagine[i][j], 3, 1, fisierImagine);

    unsigned int i2 = 1, j2= 1;
    unsigned int contor = 1;

    //--------------------------------------------------------colorarea ramelor--------------------------

    for (i = 1; i<= inaltime - inaltimeSablon; i++)
    {
        for (j = 1; j <= latime - latimeSablon; j++)
        {
            if ( tablouTemplate[contor].corelatia > ps)
            {
                //stanga
                for (i2 = tablou[contor].ly1; i2 <= tablou[contor]. r1y; i2++)
                    matriceImagine[i2][tablou[contor].lx1] = culoare;

                //jos
                for (j2 = tablou[contor].lx1; j2 <= tablou[contor].r1x; j2++)
                    matriceImagine[tablou[contor].ly1][j2] = culoare;

                //dreapta
                for (i2 = tablou[contor].ly1; i2<= tablou[contor].r1y; i2++)
                    matriceImagine[i2][tablou[contor].r1x] = culoare;

                //sus
                for (j2 = tablou[contor].lx1; j2<= tablou[contor].r1x; j2++)
                    matriceImagine[tablou[contor].r1y][j2] = culoare;

                /*
                //stanga
                for (i2 = i; i2 <= inaltimeSablon + i - 1; i2++)
                    matriceImagine[i2][j] = culoare;

                //jos
                for (j2 = j; j2 <= latimeSablon + j - 1; j2++)
                    matriceImagine[i][j2] = culoare;

                //dreapta
                for (i2 = i; i2 <= inaltimeSablon + i - 1; i2++)
                    matriceImagine[i2][latimeSablon + j - 1] = culoare;

                //sus
                for (j2 = j; j2 <= latimeSablon + j - 1; j2++)
                    matriceImagine[inaltimeSablon + i - 1][j2] = culoare;

                 */
            }

            tablou[contor].cifra = cifra;
            contor++;
        }
    }

    //---------------------------------------------am desenat ramele-----------------

    //---------------------------------------------acum punem in tablou si salvam in imagine------------

    unsigned int *tablouCuRame = calloc(latime*inaltime, sizeof(unsigned int));
    contor = 1;

    for (i = 1; i <= inaltime; i++)
        for (j = 1; j <= latime; j++)
        {
            tablouCuRame[contor] = matriceImagine[i][j];
            contor++;
        }

    salvareInMemorieExterna(caleImagine,tablouCuRame,headerCeVine);

    fclose(fisierImagine);
    fclose(fisierSablon);
}

//-----------------------------------------------------------------------------------------------------

//----------------------------------------------- Eliminare non-maxime --------------------------------

//-----------------------------------------------------------------------------------------------------

/**
 * Intoarce intr-un tabou toti indicii posibili ai ferestrelor
 * @param caleImagine
 * @param caleSablon
 * @return tablou
 */
struct Template *obtinereIndiciFerestre(char *caleImagine, char *caleSablon)
{
    FILE *fisierImagine = fopen(caleImagine, "rb");
    FILE *fisierSablon  = fopen(caleSablon, "rb");

    if (fisierImagine == NULL)
    {
        printf("fisierImagine este gol sau nu a putut fi deschis");
        return 0;
    }

    if (fisierSablon == NULL)
    {
        printf("fisierSablon este gol sau nu a putut fi deschis");
        return 0;
    }

    //----------------------------------------luam latime si inaltime sablon --------------------

    fseek(fisierSablon, 18, SEEK_SET);
    unsigned int latimeSablon = 0;
    fread(&latimeSablon, sizeof(latimeSablon), 1, fisierSablon);

    unsigned int inaltimeSablon = 0;
    fread(&inaltimeSablon, sizeof(inaltimeSablon), 1, fisierSablon);

    fseek(fisierSablon, 0, SEEK_SET);

    unsigned char *headerSablon = calloc(54, sizeof(unsigned char));
    fread(headerSablon, 54, 1, fisierSablon);

    fseek(fisierSablon, 54, SEEK_SET);

    unsigned char *headerCeVine = obtineHeader(caleImagine);
    unsigned int x = headerCeVine[18];
    unsigned int y = headerCeVine[22];
    unsigned int a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c; //latime

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f; //inaltime

    fseek(fisierImagine, 54, SEEK_SET);

    unsigned int latime = x, inaltime = y;
    unsigned int i = 1, j = 1, contor = 1;

//    struct Template *tablou = calloc((inaltime - inaltimeSablon)*(latime - latimeSablon), sizeof(struct Template));

    struct Template *tablou = calloc(latime * inaltime, sizeof(struct Template));


    for (i = 1; i<= inaltime - inaltimeSablon; i++)
        for (j = 1; j <= latime - latimeSablon; j++)
        {
            tablou[contor].nrFereastra = contor;

            tablou[contor].ly1 = i;
            tablou[contor].r1y = inaltimeSablon + i - 1;
            tablou[contor].lx1 = j;
            tablou[contor].r1x = latimeSablon   + j - 1;

            contor++;
        }

    return tablou;
}

/**
 * Sorteaza descrescator un tablou
 * @param a
 * @param b
 * @return tablou
 */
int cmp( const void *a, const void *b)
{
    struct Template va, vb;
    unsigned int aux = 0;

    va = *(struct Template *)a;
    vb = *(struct Template *)b;

    if( va.corelatia < vb.corelatia)
        return 1;

    if( va.corelatia > vb.corelatia)
        return -1;

    return 0;
}

//-------------------------------------------Intreb daca se suprapun----------------------------------

/**
 * - Dati fiind indicii a doua ferestre returneaza 0 sau 1
 * dupa cum ferestrele nu se suprapun sau se suprapun
 * @param lx1
 * @param ly1
 * @param r1x
 * @param r1y
 * @param lx2
 * @param ly2
 * @param r2x
 * @param r2y
 * @return 0 sau 1
 */
unsigned int intrebareSuprapunere(unsigned int lx1, unsigned int ly1, unsigned r1x, unsigned int r1y,
                                  unsigned int lx2, unsigned int ly2, unsigned r2x, unsigned int r2y) {
    //stanga
    if (lx1 > r2x || lx2 > r1x)
        return 0;  //fals

    // deasupra
    if (ly1 < r2y || ly2 < r1y)
        return 0;

    return 1;
}

//----------------------------------------------------aici calculez daca e mai mare > 0.2 suprapunerea----

/**
 * Returneaza rezultatul suprapunerii a doua ferestre
 * @param lx1
 * @param ly1
 * @param r1x
 * @param r1y
 * @param lx2
 * @param ly2
 * @param r2x
 * @param r2y
 * @return rezultat
 */
double rezultatSuprapunere( unsigned int lx1, unsigned int ly1, unsigned r1x, unsigned int r1y,
                            unsigned int lx2, unsigned int ly2, unsigned r2x, unsigned int r2y)
{
    double aria1 = ((lx1 - r1x ) * (ly1 - r1y ));

    double aria2 = ((lx2 - r2x ) * (ly2 - r2y ));

    unsigned int minr1x = 0;
    unsigned int maxl1x = 0;

    unsigned int minr1y = 0;
    unsigned int maxl1y = 0;

    if ( r1x < r2x)
        minr1x = r1x;
    else
        minr1x = r2x;

    if (lx1 > lx2)
        maxl1x = lx1;
    else
        maxl1x = lx2;

    if (r1y < r2y)
        minr1y = r1y;
    else
        minr1y = r2y;

    if (ly1 > ly2)
        maxl1y = ly1;
    else
        maxl1y = ly2;

    double ariaIntersectiei = 0;

    ariaIntersectiei = ((minr1x - maxl1x) * (minr1y - maxl1y));

    return ((ariaIntersectiei)/ (aria1 + aria2 - ariaIntersectiei));
}

/**
 * Elimina dintr-un tablou ferestrele ce se suprapun >0.2
 * @param tablouDetectii
 * @param dimTD
 * @return tablou
 */
struct Template *eliminareSuprapuneri(struct Template *tablouDetectii, unsigned int dimTD)
{

    unsigned int i = 1, j = 1, k = 1, copieDimTD = dimTD;

        for ( i = 1; i <= dimTD - 1; i++)
        {
            for (j = i + 1; j <= dimTD; j++)
            {
                if (tablouDetectii[i].corelatia > 0.5 && tablouDetectii[j].corelatia > 0.5)
                {

                    if (tablouDetectii[i].corelatia >= tablouDetectii[j].corelatia)
                    {
                        if (intrebareSuprapunere(
                                tablouDetectii[i].lx1,
                                tablouDetectii[i].r1y,
                                tablouDetectii[i].r1x,
                                tablouDetectii[i].ly1,
                                tablouDetectii[j].lx1,
                                tablouDetectii[j].r1y,
                                tablouDetectii[j].r1x,
                                tablouDetectii[j].ly1) == 1)
                        {
                            if (rezultatSuprapunere(
                                    tablouDetectii[i].lx1,
                                    tablouDetectii[i].r1y,
                                    tablouDetectii[i].r1x,
                                    tablouDetectii[i].ly1,
                                    tablouDetectii[j].lx1,
                                    tablouDetectii[j].r1y,
                                    tablouDetectii[j].r1x,
                                    tablouDetectii[j].ly1) > 0.2)
                            {
                                for (k = j; k <= dimTD - 1; k++)
                                {
                                    tablouDetectii[k].cifra = tablouDetectii[k + 1].cifra;

                                    tablouDetectii[k].corelatia = tablouDetectii[k + 1].corelatia;
                                    tablouDetectii[k].r1x = tablouDetectii[k + 1].r1x;
                                    tablouDetectii[k].r1y = tablouDetectii[k + 1].r1y;
                                    tablouDetectii[k].lx1 = tablouDetectii[k + 1].lx1;
                                    tablouDetectii[k].ly1 = tablouDetectii[k + 1].ly1;
                                }

                                dimTD--;
                            }
                        }
                    }
                }
            }
        }

    return tablouDetectii;
}

/**
 * Se coloreaza imaginea doar cu ferestrele care nu se suprapun >0.2
 * @param caleImagine
 * @param tablouTemplate
 * @param caleSablon
 * @param ps
 */
void colorareNouaFereastraFinal (char *caleImagine,
                        struct Template *tablouTemplate,
                        char *caleSablon, double ps)
{
    FILE *fisierImagine = fopen(caleImagine, "rb");
    FILE *fisierSablon  = fopen(caleSablon, "rb");

    if (fisierImagine == NULL) {
        printf("fisierImagine este gol sau nu a putut fi deschis");
        return ;
    }

    if (fisierSablon == NULL) {
        printf("fisierSablon este gol sau nu a putut fi deschis");
        return ;
    }

    unsigned int rosu      = 16711680;
    unsigned int galben    = 16776960;
    unsigned int verde     = 65280   ;
    unsigned int cyan      = 65535   ;
    unsigned int magenta1  = 16711935;
    unsigned int albastru1 = 255     ;
    unsigned int argintiu  = 12632256;
    unsigned int albastru2 = 16747520;
    unsigned int magenta2  = 8388736;
    unsigned int albastru3 = 8388608;

    fseek(fisierSablon, 18, SEEK_SET);
    unsigned int latimeSablon = 0;
    fread(&latimeSablon, sizeof(latimeSablon), 1, fisierSablon);

    unsigned int inaltimeSablon = 0;
    fread(&inaltimeSablon, sizeof(inaltimeSablon), 1, fisierSablon);

    fseek(fisierSablon, 0, SEEK_SET);

    unsigned char *headerSablon = calloc(54, sizeof(unsigned char));
    fread(headerSablon, 54, 1, fisierSablon);

    //-------------------------------------------------------avem latime si inaltime sablon-----------------------

    unsigned char *headerCeVine = obtineHeader(caleImagine);
    unsigned int a, b, c;

    unsigned int x = headerCeVine[18];

    a = (unsigned int) (headerCeVine[19]);
    a <<= 8;
    b = (unsigned int) (headerCeVine[20]);
    b <<= 16;
    c = (unsigned int) (headerCeVine[21]);
    c <<= 24;
    x += a + b + c; //latime

    unsigned int y = headerCeVine[22];
    unsigned int d, e, f;

    d = (unsigned int) (headerCeVine[23]);
    d <<= 8;
    e = (unsigned int) (headerCeVine[24]);
    e <<= 16;
    f = (unsigned int) (headerCeVine[25]);
    f <<= 24;
    y += d + e + f; //inaltime

    fseek(fisierImagine, 54, SEEK_SET);

    unsigned int latime = x, inaltime = y;

    unsigned int latimeTest = 0, inaltimeTest = 0;

    // ---------------------------------------------------avem latime si inaltime imagine--------------------------

    unsigned int i = 1, j = 1, iTI = 1;

    unsigned int **matriceImagine = calloc(latime * inaltime, sizeof(unsigned int *));

    for (i = 1; i <= inaltime; i++)
        matriceImagine[i] = calloc(latime, sizeof(unsigned int));

    fseek(fisierImagine, 54,SEEK_SET);
    unsigned int padding = 0;

    if (latime % 4 != 0)
        padding = 4 - (3 * latime) % 4;
    else
        padding = 0;

    for (i = 1; i <= inaltime; i++)
    {
        for (j = 1; j <= latime; j++)
        {
            fread(&matriceImagine[i][j], 3, 1, fisierImagine);
        }
        fseek(fisierSablon,padding,SEEK_CUR);
    }

    unsigned int i2 = 1, j2= 1;
    unsigned int contor = 1, culoare = 0;

    //--------------------------------------------------------colorarea ramelor--------------------------

    for (i = 1; i<= inaltime - inaltimeSablon; i++)
    {
        for (j = 1; j <= latime - latimeSablon; j++)
        {
            if ( tablouTemplate[contor].corelatia > ps)
            {
                switch (tablouTemplate[contor].cifra)
                {
                    case 0:
                        culoare = rosu;
                        break;
                    case 1:
                        culoare = galben;
                        break;
                    case 2:
                        culoare = verde;
                        break;
                    case 3:
                        culoare = cyan;
                        break;
                    case 4:
                        culoare = magenta1;
                        break;
                    case 5:
                        culoare = albastru1;
                        break;
                    case 6:
                        culoare = argintiu;
                        break;
                    case 7:
                        culoare = albastru2;
                        break;
                    case 8:
                        culoare = magenta2;
                        break;
                    case 9:
                        culoare = albastru3;
                        break;

                    default:
                        culoare = albastru1;
                        break;
                }

                //stanga
                for (i2 = tablouTemplate[contor].ly1; i2 <= tablouTemplate[contor]. r1y; i2++)
                    matriceImagine[i2][tablouTemplate[contor].lx1] = culoare;

                //jos
                for (j2 = tablouTemplate[contor].lx1; j2 <= tablouTemplate[contor].r1x; j2++)
                    matriceImagine[tablouTemplate[contor].ly1][j2] = culoare;

                //dreapta
                for (i2 = tablouTemplate[contor].ly1; i2<= tablouTemplate[contor].r1y; i2++)
                    matriceImagine[i2][tablouTemplate[contor].r1x] = culoare;

                //sus
                for (j2 = tablouTemplate[contor].lx1; j2<= tablouTemplate[contor].r1x; j2++)
                    matriceImagine[tablouTemplate[contor].r1y][j2] = culoare;
            }
            contor++;
        }
    }

    //---------------------------------------------am desenat ramele-----------------

    //---------------------------------------------acum punem in tablou si salvam in imagine------------

    unsigned int *tablouCuRame = calloc(latime*inaltime, sizeof(unsigned int));
    contor = 1;

    for (i = 1; i <= inaltime; i++)
        for (j = 1; j <= latime; j++)
        {
            tablouCuRame[contor] = matriceImagine[i][j];
            contor++;
        }

    salvareInMemorieExterna(caleImagine,tablouCuRame,headerCeVine);

    fclose(fisierImagine);
    fclose(fisierSablon);
}

int main()
{
    //---------------------------------------------Criptare, decriptare si testul chi patrat----------------------------


    criptareImagine(
            "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\criptografie\\peppers.bmp",
            "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\criptografie\\cr_peppers.bmp",
            "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\criptografie\\secret_key.txt");

    decriptareImagine(
            "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\criptografie\\peppers.bmp",
            "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\criptografie\\cr_peppers.bmp",
            "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\criptografie\\decriptcr_pepppppers.bmp",
            "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\criptografie\\secret_key.txt");

    testulChiPatrat(
            "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\criptografie\\cr_peppers.bmp");

    //---------------------------------------------Criptare, decriptare si testul chi patrat----------------------------

    //---------------------------------------------Partea a doua a proiectului------------------------------------------

    unsigned int rosu      = 16711680;
    unsigned int galben    = 16776960;
    unsigned int verde     = 65280   ;
    unsigned int cyan      = 65535   ;
    unsigned int magenta1  = 16711935;
    unsigned int albastru1 = 255     ;
    unsigned int argintiu  = 12632256;
    unsigned int albastru2 = 16747520;
    unsigned int magenta2  = 8388736;
    unsigned int albastru3 = 8388608;

    double ps = 0.5;
    unsigned int i = 1, j = 1;

    struct Template *tablou0 = NULL;
    struct Template *tablou1 = NULL;
    struct Template *tablou2 = NULL;
    struct Template *tablou3 = NULL;
    struct Template *tablou4 = NULL;
    struct Template *tablou5 = NULL;
    struct Template *tablou6 = NULL;
    struct Template *tablou7 = NULL;
    struct Template *tablou8 = NULL;
    struct Template *tablou9 = NULL;

    unsigned int numar0 = 0, numar1 = 0, numar2 = 0, numar3 = 0, numar4 = 0;
    unsigned int numar5 = 0, numar6 = 0, numar7 = 0, numar8 = 0, numar9 = 0;

    //-----------------------------  Functia calcululCorelatiei pune in fiecare tablou corelatiile cifrei respective  ----------------

    tablou0 = calcululCorelatiei("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\test.bmp","C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra0.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", 0, &numar0);
    tablou1 = calcululCorelatiei("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\test.bmp","C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra1.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", 1, &numar1);
    tablou2 = calcululCorelatiei("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\test.bmp","C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra2.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", 2, &numar2);
    tablou3 = calcululCorelatiei("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\test.bmp","C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra3.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", 3, &numar3);
    tablou4 = calcululCorelatiei("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\test.bmp","C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra4.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", 4, &numar4);
    tablou5 = calcululCorelatiei("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\test.bmp","C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra5.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", 5, &numar5);
    tablou6 = calcululCorelatiei("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\test.bmp","C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra6.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", 6, &numar6);
    tablou7 = calcululCorelatiei("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\test.bmp","C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra7.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", 7, &numar7);
    tablou8 = calcululCorelatiei("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\test.bmp","C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra8.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", 8, &numar8);
    tablou9 = calcululCorelatiei("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\test.bmp","C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra9.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", 9, &numar9);

    //------------------------------   Acum se pun ramele in imagine cand corelatia > 0.5  -----------------------

    struct Template *tablouIndici = NULL;
    tablouIndici = obtinereIndiciFerestre("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp", "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra0.bmp");

//    colorareFereastra("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp",rosu,      tablou0, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra0.bmp", ps, tablouIndici, 0);
//    colorareFereastra("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp",galben,    tablou1, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra1.bmp", ps, tablouIndici, 1);
//    colorareFereastra("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp",verde,     tablou2, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra2.bmp", ps, tablouIndici, 2);
//    colorareFereastra("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp",cyan,      tablou3, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra3.bmp", ps, tablouIndici, 3);
//    colorareFereastra("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp",magenta1,  tablou4, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra4.bmp", ps, tablouIndici, 4);
//    colorareFereastra("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp",albastru1, tablou5, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra5.bmp", ps, tablouIndici, 5);
//    colorareFereastra("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp",argintiu,  tablou6, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra6.bmp", ps, tablouIndici, 6);
//    colorareFereastra("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp",albastru2, tablou7, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra7.bmp", ps, tablouIndici, 7);
//    colorareFereastra("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp",magenta2,  tablou8, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra8.bmp", ps, tablouIndici, 8);
//    colorareFereastra("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp",albastru3, tablou9, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra9.bmp", ps, tablouIndici, 9);

    //------------------------------  Acum eliminam non-maximele  --------------------------------------------------

    //------------------------------  Punem toate corelatiile existente intr-un singur tablou  ---------------------

    struct Template *toateDetectiile = calloc(numar0 * 10, sizeof(struct Template));
    unsigned int contor = 1;

    for ( j = 1; j<= numar0; j++)
    {
        toateDetectiile[contor].corelatia           = tablou0[j].corelatia;
        toateDetectiile[contor].cifra               = tablou0[j].cifra;

        toateDetectiile[contor].lx1 = tablou0[j].lx1;
        toateDetectiile[contor].ly1  = tablou0[j].ly1;
        toateDetectiile[contor].r1y  = tablou0[j].r1y;
        toateDetectiile[contor].r1x = tablou0[j].r1x;

        contor++;
    }
    free(tablou0);

    for ( j = 1; j<= numar1; j++)
    {
        toateDetectiile[contor].corelatia           = tablou1[j].corelatia;
        toateDetectiile[contor].cifra               = tablou1[j].cifra;

        toateDetectiile[contor].lx1 = tablou1[j].lx1;
        toateDetectiile[contor].ly1  = tablou1[j].ly1;
        toateDetectiile[contor].r1y  = tablou1[j].r1y;
        toateDetectiile[contor].r1x = tablou1[j].r1x;

        contor++;
    }
    free(tablou1);

    for ( j = 1; j<= numar2; j++)
    {
        toateDetectiile[contor].corelatia           = tablou2[j].corelatia;
        toateDetectiile[contor].cifra               = tablou2[j].cifra;

        toateDetectiile[contor].lx1 = tablou2[j].lx1;
        toateDetectiile[contor].ly1  = tablou2[j].ly1;
        toateDetectiile[contor].r1y  = tablou2[j].r1y;
        toateDetectiile[contor].r1x = tablou2[j].r1x;

        contor++;
    }
    free(tablou2);

    for ( j = 1; j<= numar3; j++)
    {
        toateDetectiile[contor].corelatia           = tablou3[j].corelatia;
        toateDetectiile[contor].cifra               = tablou3[j].cifra;

        toateDetectiile[contor].lx1 = tablou3[j].lx1;
        toateDetectiile[contor].ly1  = tablou3[j].ly1;
        toateDetectiile[contor].r1y  = tablou3[j].r1y;
        toateDetectiile[contor].r1x = tablou3[j].r1x;

        contor++;
    }
    free(tablou3);

    for ( j = 1; j<= numar4; j++)
    {
        toateDetectiile[contor].corelatia           = tablou4[j].corelatia;
        toateDetectiile[contor].cifra               = tablou4[j].cifra;

        toateDetectiile[contor].lx1 = tablou4[j].lx1;
        toateDetectiile[contor].ly1  = tablou4[j].ly1;
        toateDetectiile[contor].r1y  = tablou4[j].r1y;
        toateDetectiile[contor].r1x = tablou4[j].r1x;

        contor++;
    }
    free(tablou4);

    for ( j = 1; j<= numar5; j++)
    {
        toateDetectiile[contor].corelatia           = tablou5[j].corelatia;
        toateDetectiile[contor].cifra               = tablou5[j].cifra;

        toateDetectiile[contor].lx1 = tablou5[j].lx1;
        toateDetectiile[contor].ly1  = tablou5[j].ly1;
        toateDetectiile[contor].r1y  = tablou5[j].r1y;
        toateDetectiile[contor].r1x = tablou5[j].r1x;

        contor++;
    }
    free(tablou5);

    for ( j = 1; j<= numar6; j++)
    {
        toateDetectiile[contor].corelatia           = tablou6[j].corelatia;
        toateDetectiile[contor].cifra               = tablou6[j].cifra;

        toateDetectiile[contor].lx1 = tablou6[j].lx1;
        toateDetectiile[contor].ly1  = tablou6[j].ly1;
        toateDetectiile[contor].r1y  = tablou6[j].r1y;
        toateDetectiile[contor].r1x = tablou6[j].r1x;

        contor++;
    }
    free(tablou6);

    for ( j = 1; j<= numar7; j++)
    {
        toateDetectiile[contor].corelatia            = tablou7[j].corelatia;
        toateDetectiile[contor].cifra                = tablou7[j].cifra;

        toateDetectiile[contor].lx1 = tablou7[j].lx1;
        toateDetectiile[contor].ly1  = tablou7[j].ly1;
        toateDetectiile[contor].r1y  = tablou7[j].r1y;
        toateDetectiile[contor].r1x = tablou7[j].r1x;

        contor++;
    }
   free(tablou7);

    for ( j = 1; j<= numar8; j++)
    {
        toateDetectiile[contor].corelatia           = tablou8[j].corelatia;
        toateDetectiile[contor].cifra               = tablou8[j].cifra;

        toateDetectiile[contor].lx1 = tablou8[j].lx1;
        toateDetectiile[contor].ly1  = tablou8[j].ly1;
        toateDetectiile[contor].r1y  = tablou8[j].r1y;
        toateDetectiile[contor].r1x = tablou8[j].r1x;

        contor++;
    }
    free(tablou8);

    for ( j = 1; j<= numar9; j++)
    {
        toateDetectiile[contor].corelatia           = tablou9[j].corelatia;
        toateDetectiile[contor].cifra               = tablou9[j].cifra;

        toateDetectiile[contor].lx1 = tablou9[j].lx1;
        toateDetectiile[contor].ly1  = tablou9[j].ly1;
        toateDetectiile[contor].r1y  = tablou9[j].r1y;
        toateDetectiile[contor].r1x = tablou9[j].r1x;

        contor++;
    }
    free(tablou9);

    qsort(toateDetectiile, numar0 * 10, sizeof(struct Template), cmp);

    i = 1;

    //--------------------------------  In tabloul toateDetectiile am acum toate corelatiile tablourilor  --------------

    //--------------------------------  Acum eliminam suprapunerile ----------------------------------------------------

    struct Template *tablouFinal = calloc(numar0 , sizeof(struct Template));

    i = 1;
    j = 1;

    while (toateDetectiile[i].corelatia > 0.5)
    {
        tablouFinal[i].corelatia           = toateDetectiile[j].corelatia;
        tablouFinal[i].cifra               = toateDetectiile[j].cifra;

        tablouFinal[i].lx1  = toateDetectiile[j].lx1;
        tablouFinal[i].ly1  = toateDetectiile[j].ly1;
        tablouFinal[i].r1y  = toateDetectiile[j].r1y;
        tablouFinal[i].r1x  = toateDetectiile[j].r1x;

        i ++;
        j ++;
    }

    tablouFinal = eliminareSuprapuneri(toateDetectiile, i);

    //--------------------------------  Acum coloram doar ce a mai ramas in toateDetectiile ----------------------------

    colorareNouaFereastraFinal("C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\imagineaGrayscale.bmp"
                               ,tablouFinal, "C:\\Users\\Gabriel Arghire\\Desktop\\Facultate\\ProiectPP arhiva\\proiectPP\\date\\recunoasterePatternuri\\cifra0.bmp"
                               , ps);

    printf("\n Am ajuns la sfarsit \n");
    return 0;
}