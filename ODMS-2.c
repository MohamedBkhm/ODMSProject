#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
typedef struct config
{
    int NB_OUTILS,NB_PLACES,NB_OPERATIONS,NB_DELTA;
    int *OUTIL, *TPS;
} Config;

int calcul(Config c, int *magasin);

Config lire_donnees()
{
    FILE *fichier;
    int i;
    Config c;
    fichier = fopen("donneezs.txt","r");
    if(fichier==NULL)
    {
        printf("Impossible d'ouvrir le fichier\n");
        exit(0);
    }

    fscanf(fichier,"%d",&c.NB_OUTILS);
    fscanf(fichier,"%d",&c.NB_PLACES);
    fscanf(fichier,"%d",&c.NB_OPERATIONS);
    fscanf(fichier,"%d",&c.NB_DELTA);

    c.OUTIL = malloc(c.NB_OPERATIONS * sizeof(int));
    c.TPS = malloc(c.NB_OPERATIONS * sizeof(int));

    for(i=0; i<c.NB_OPERATIONS ; i++)
    {
        fscanf(fichier,"%d",&c.OUTIL[i]);
        fscanf(fichier,"%d",&c.TPS[i]);
    }

    return c;
}



int absolue(int x)
{
    if (x<0)
        return -x;
    else
        return x;
}

double acceptance_probability(int old_cost, int new_cost, double T)
{
    return exp((old_cost-new_cost)/T);
}

int *neighbor(Config c,int *magasin)
{
    int *cpy_magasin= malloc(c.NB_PLACES * sizeof(int));
    for (int k = 0; k < c.NB_PLACES; k++)
    {
        cpy_magasin[k] = magasin[k];
    }
    int r1;
    do
    {
        r1 = rand() % c.NB_PLACES;
    }
    while(cpy_magasin[r1]==0);

    int r2;

    if(rand()%1 > 0.5) r2 = r1+1 % c.NB_PLACES;
    else r2 = r1-1 % c.NB_PLACES;
    if(r2<0) r2=c.NB_PLACES-1;



    int temp = cpy_magasin[r1];
    cpy_magasin[r1]=cpy_magasin[r2];
    cpy_magasin[r2]=temp;

    return cpy_magasin;
}

double r2()
{
    return 0.5 ;
}

int *anneal(Config c, int *magasin)
{


    int old_cost = calcul(c,magasin);
    int *new_sol= malloc(c.NB_PLACES * sizeof(int));
    int *best_sol= malloc(c.NB_PLACES * sizeof(int));
    int new_cost,i,best_cost;
    double ap;
    double T = 1;
    double T_min = 0.01;
    double alpha = 0.9;
    best_cost=old_cost;
    while(T > T_min)
    {
        i = 1;
        while(i <= 50)
        {
            //printf("\n\ni=%d   T = %f \n\n ",i,T);
            new_sol = neighbor(c,magasin);
            new_cost = calcul(c,new_sol);
            if(new_cost<best_cost)
            {
                best_cost = new_cost;
                best_sol = new_sol;
                if(best_cost==0) T=T_min;
            }
            ap = acceptance_probability(old_cost, new_cost, T);
            printf("\nap = %f on compare a 0.1\n", ap);
            printf("new_cost=%d    old_cost=%d\n",new_cost,old_cost);
            ap=acceptance_probability(old_cost,new_cost,T);
            if(ap>0.1)
            {
                printf("\nAccepte\n_______________________________________________________________________________\n\n");
                magasin = new_sol;
                old_cost = new_cost;

            }
            else printf("\nNon accepte\n_______________________________________________________________________________\n\n");
            i++;
            //system("pause");
        }
        printf("\n\n\nMeilleur solution %d\nNous allons continuer avec la configuration qui coute %d",best_cost,old_cost);
        printf("\nMagasin avec lequel nous allons continuer : \n ");
        for(int k=0; k<c.NB_PLACES; k++)
        {
            printf("%d ",magasin[k]);
        }
        printf("\n\n");

        //system("pause");
        T = T*alpha;
    }
    printf("\n\n\n ******** FIN DE LA RECHERCHE ********\n\n\n Meilleur temps trouve %d\nMeilleure configuration : ",best_cost);
    for (int k = 0; k < c.NB_PLACES; k++)
    {
        printf("%d ",best_sol[k]);
    }
    printf("\n\n");
    free(magasin);
    return best_sol;
}

int calcul(Config c, int *magasin)
{
    int i;
    int *first_magasin= malloc(c.NB_PLACES * sizeof(int));
    for (int k = 0; k < c.NB_PLACES; k++)
    {
        first_magasin[k] = magasin[k];
    }
    printf("\nCalcul du cout avec le magasin suivant : \n ");
    for(int k=0; k<c.NB_PLACES; k++)
    {
        printf("%d ",magasin[k]);
    }

    int *magasin_old= malloc(c.NB_PLACES * sizeof(int));
    for (int k = 0; k < c.NB_PLACES; k++)
    {
        magasin_old[k] = magasin[k];
    }



    int j,k,z,x,cout, cout_total=0;
    int outil_current=0;
    int tmp;

    for(i=0; i<c.NB_OPERATIONS ; i++)
    {
        cout = 0;
        j=0;
        if(c.OUTIL[i]!=magasin[0])
        {
           // printf("OUTIL %d\n",c.OUTIL[i]);
            while(magasin[j]!=c.OUTIL[i])
            {
                j++;
            }
            if(j>(c.NB_PLACES-1)/2)
                j = j - c.NB_PLACES;
            //printf("nous allons decaler de %d\n\n",j);
        }
        for(k=0; k<c.NB_PLACES ; k++)
        {
            if (j < 0)
                magasin[(k-j)%c.NB_PLACES] =  magasin_old[k];
            else if (j > 0)
                magasin[k] = magasin_old[(k+j)%c.NB_PLACES];
        }

        tmp = outil_current;
        outil_current = magasin[0];
        magasin[0] = tmp;

        /*printf("nous utilisons actuellement l'outil %d\n",outil_current);
        for(z=0; z<c.NB_PLACES ; z++)
        {
            printf("%d ",magasin[z]);
        }*/
        if(i==0)
        {
            cout_total = cout_total + c.NB_DELTA * absolue(j);
            //printf("\nLe temps de mouvement est de %d -> cout %d\n",c.NB_DELTA * absolue(j),cout_total);
        }
        else
        {
            if(c.NB_DELTA * absolue(j) - c.TPS[i-1] <= 0)
            {
               // printf("\nLe temps de mouvement est de %d et le temps d'usinage %d -> temps masque\n",c.NB_DELTA * absolue(j),c.TPS[i-1]);
            }
            else
            {
                cout = c.NB_DELTA * absolue(j) - c.TPS[i-1];
                //printf("\nLe temps de mouvement est de %d et le temps d'usinage %d -> cout %d\n",c.NB_DELTA * absolue(j),c.TPS[i-1],cout);
            }

        }
       // printf("\n\n\n");

        for (int k = 0; k < c.NB_PLACES; k++)
        {
            magasin_old[k] = magasin[k];
        }

        cout_total += cout;
    }

    cout = 0;
    j=0;
    x=c.NB_OPERATIONS-1;
    if(magasin[0]!=0)
    {
        //printf("Nous allons nous positionner a un emplacement vide");
        while(magasin[j]!=0)
        {
            j++;
        }
        while(magasin[x]!=0)
        {
            x--;
        }
        x = c.NB_OPERATIONS-x;
        if (x<j)
            j=-x;
        //printf("nous allons decaler de %d\n\n",j);
    }
    for(k=0; k<c.NB_PLACES ; k++)
    {
        if (j < 0)
            magasin[(k-j)%c.NB_PLACES] =  magasin_old[k];
        else if (j > 0)
            magasin[k] = magasin_old[(k+j)%c.NB_PLACES];
    }

    magasin[0] = outil_current;
    outil_current = 0;


    /*for(z=0; z<c.NB_PLACES ; z++)
    {
         printf("%d ",magasin[z]);
    }*/

    if(c.NB_DELTA * absolue(j) - c.TPS[i-1] <= 0)
    {
        //printf("\nLe temps de mouvement est de %d et le temps d'usinage %d -> temps masque\n",c.NB_DELTA * absolue(j),c.TPS[i-1]);
    }
    else
    {
        cout = c.NB_DELTA * absolue(j) - c.TPS[i];
        //printf("\nLe temps de mouvement est de %d et le temps d'usinage %d -> cout %d\n",c.NB_DELTA * absolue(j),c.TPS[i-1],cout);
    }


    for (int k = 0; k < c.NB_PLACES; k++)
    {
        magasin_old[k] = magasin[k];
    }
    printf("\n*** COUT TOTAL = %d ***\n\n",cout_total);
    //system("pause");
    for (int k = 0; k < c.NB_PLACES; k++)
    {
        magasin[k] = first_magasin[k];
    }
    return cout_total;

}



void shuffle(int *array, size_t n)
{
    if (n > 1)
    {
        size_t i;
        for (i = 0; i < n - 1; i++)
        {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

void mag_init(int *mag, Config c)
{
    int i, j;

    for (i=0; i<c.NB_PLACES; i++)
    {
        mag[i] = 0;
    }
    for (j=0; j<c.NB_OUTILS; j++)
    {
        mag[j] = (int)j+1;
    }
}

void mag_init2(int *mag, Config c)
{
    int i=0;
    int j=0;
    int *outil_used= malloc(c.NB_OUTILS * sizeof(int));

    for (i=0; i<c.NB_OUTILS; i++)
    {
        outil_used[i] = 0;
    }

    for (i=0; i<c.NB_OUTILS; i++)
    {
        printf("%d ",outil_used[i]);
    }

    for (i=0; i<c.NB_PLACES; i++)
    {
        mag[i] = 0;
    }

     printf("mag =  ");

    for (i=0; i<c.NB_PLACES; i++)
    {
        printf("%d ",mag[i]);
    }

    int used,z;
    z=0;
    i=0;
    while(i<c.NB_OPERATIONS)
    {
        used = 0;
        for (j=0; j<c.NB_OUTILS; j++)
        {
            if(outil_used[j]==c.OUTIL[i])
            {
                used = 1;
            }
        }
        if(used == 0)
        {
            outil_used[z] = c.OUTIL[i];
            mag[z]=c.OUTIL[i];
            i++;
            z++;
        }
        else i++;
    }
}

void mag_init3(int *mag, Config c)
{

        for (int i=0; i<c.NB_PLACES; i++)
    {
        mag[i] = 0;
    }

    for(int i=0; i<c.NB_PLACES ; i++)
    {

           if(i<c.NB_OPERATIONS) mag[i]=c.OUTIL[i];
    }
}

int main()
{
    srand(time(NULL));

    int i;
    Config c = lire_donnees();
    printf("%d\n%d\n%d\n%d\n",c.NB_OUTILS,c.NB_PLACES,c.NB_OPERATIONS,c.NB_DELTA);
    for(i=0; i<c.NB_OPERATIONS ; i++)
    {
        printf("%d %d\n",c.OUTIL[i],c.TPS[i]);
    }
    printf("\n\n");

    int *magasin= malloc(c.NB_PLACES * sizeof(int));
    mag_init(magasin, c);
    magasin=anneal(c, magasin);
    free(magasin);
    return 0;
}
