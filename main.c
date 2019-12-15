#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

typedef struct{
    int magassag;
    int szelesseg;
    char **palya;
}Labirintus;


typedef struct{
    int sor;
    int oszlop;
    int xirany;
    int yirany;
}Koordinata;

typedef enum Irany{fel, le, jobbra, balra} Irany;

typedef enum Eredmeny{fal, kincs, kijarat} Eredmeny;


void delay(double masodperc)
{
	/**
	A kesleltetes eleresehez a gep szamolja az idot, csak ezt masodpercbe kell valtani
	*/
	// A secundumot milli_secondumba konvertalni
	double millimasodperc = 1000 * masodperc;

	clock_t start_time = clock();

	// addig noveljuk az idot, amig el nem erjuk a konvertalt millisecet
	while (clock() < start_time + millimasodperc);
}

Labirintus beolvas(char * filename){
    FILE * fp;
    fp=fopen(filename, "r");

    int magassag;
    int szelesseg;
    fscanf(fp, "%d %d\n", &magassag, &szelesseg);
    Labirintus start;
    szelesseg++;
    start.magassag=magassag;
    start.szelesseg=szelesseg;

    start.palya=(char**)malloc(sizeof(char*)*magassag);
    start.palya[0]=(char*)malloc(sizeof(char)*magassag*szelesseg);
    for(int i=1; i<magassag; i++){
        start.palya[i]=start.palya[0]+i*szelesseg;
    }
    char c;
        for(int i=0; i<magassag; i++){
            for(int j=0; j<szelesseg; j++){
                fscanf(fp, "%c", &c);
                start.palya[i][j]=c;
            }
        }
    return start;
}

Koordinata jatekos(Labirintus palya){
    Koordinata jatekose;
    jatekose.sor=-1;
    jatekose.oszlop=-1;
    for(int i=0; i<palya.magassag; i++){
        for(int j=0; j<palya.szelesseg; j++){
            if(palya.palya[i][j]=='@'){
                jatekose.sor=i;
                jatekose.oszlop=j;
                return jatekose;
            }
        }
    }
    return jatekose;
}

int kincsek(Labirintus palya){
    int db=0;
    for(int i=0; i<palya.magassag; i++){
        for(int j=0; j<palya.szelesseg; j++){
            if(palya.palya[i][j]=='$')
                db++;
        }
    }
    return db;
}

void kiir(Labirintus palya){
    for(int i=0; i<palya.magassag; i++){
        for(int j=0; j<palya.szelesseg; j++){
            printf("%c", palya.palya[i][j]);
        }
    }
}

bool mehete(Labirintus palya, int merre){
    Koordinata player=jatekos(palya);
    if(player.oszlop==0){
        if(merre==balra)
            return false;
    }

    if(player.oszlop==palya.szelesseg-1)
        if(merre==jobbra)
            return false;
    if(player.sor==0)
        if(merre==fel)
            return false;
    if(player.sor==palya.magassag-1)
        if(merre==le)
            return false;

    if(merre==balra)
        if(palya.palya[player.sor][player.oszlop-1]=='#')
            return false;
    if(merre==jobbra)
        if(palya.palya[player.sor][player.oszlop+1]=='#')
            return false;
    if(merre==fel)
        if(palya.palya[player.sor-1][player.oszlop]=='#')
            return false;
    if(merre==le)
        if(palya.palya[player.sor+1][player.oszlop]=='#')
            return false;

    return true;
}

Eredmeny vizsgalat(Labirintus palya, int sor, int oszlop){
    if(palya.palya[sor][oszlop]=='$')
        return kincs;
    if(sor==0 || sor == palya.magassag-1 || oszlop==0 || oszlop==palya.szelesseg-1)
        return kijarat;
}

Eredmeny lepes(Labirintus * palya, int merre, Koordinata * player){
    if(mehete(*palya, merre)==false)
        return fal;

    Eredmeny segitseg;
    if(merre==fel){
        palya->palya[player->sor][player->oszlop]=' ';
        segitseg=vizsgalat(*palya, player->sor-1, player->oszlop);
        palya->palya[player->sor-1][player->oszlop]='@';
        player->sor--;
    }else if(merre==le){
        palya->palya[player->sor][player->oszlop]=' ';
        segitseg=vizsgalat(*palya, player->sor+1, player->oszlop);
        palya->palya[player->sor+1][player->oszlop]='@';
        player->sor++;
    }else if(merre==jobbra){
        palya->palya[player->sor][player->oszlop]=' ';
        segitseg=vizsgalat(*palya, player->sor, player->oszlop+1);
        palya->palya[player->sor][player->oszlop+1]='@';
        player->oszlop++;
    }else if(merre==balra){
        palya->palya[player->sor][player->oszlop]=' ';
        segitseg=vizsgalat(*palya, player->sor, player->oszlop-1);
        palya->palya[player->sor][player->oszlop-1]='@';
        player->oszlop--;
    }
    return segitseg;
}

void babu_rajzol(SDL_Renderer *renderer, SDL_Texture *babuk, int x, int y) {
    int MERET=79;
    /* a forras kepbol ezekrol a koordinatakrol, ilyen meretu reszletet masolunk. */
    SDL_Rect src = { 0, 0, MERET, MERET };
    /* a cel kepre, ezekre a koordinatakra masoljuk */
    SDL_Rect dest = { x*80+1, y*80+1, MERET, MERET };
    /* kepreszlet masolasa */
    SDL_RenderCopy(renderer, babuk, &src, &dest);
}


//void palyakirajzol(Labirintis palya, SDL)

/* kulon fuggvenybe, hogy olvashatobb legyen */
void sdl_init(int szeles, int magas, SDL_Window **pwindow, SDL_Renderer **prenderer) {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        SDL_Log("Nem indithato az SDL: %s", SDL_GetError());
        exit(1);
    }
    SDL_Window *window = SDL_CreateWindow("Labirintus jatek", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, szeles, magas, 0);
    if (window == NULL) {
        SDL_Log("Nem hozhato letre az ablak: %s", SDL_GetError());
        exit(1);
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if (renderer == NULL) {
        SDL_Log("Nem hozhato letre a megjelenito: %s", SDL_GetError());
        exit(1);
    }
    SDL_RenderClear(renderer);

    *pwindow = window;
    *prenderer = renderer;
}


/* ez a fuggveny hivodik meg az idozito altal.
 * betesz a feldolgozando esemenyek koze (push) egy felhasznaloi esemenyt */
Uint32 idozit(Uint32 ms, void *param) {
    SDL_Event ev;
    ev.type = SDL_USEREVENT;
    SDL_PushEvent(&ev);
    return ms;   /* ujabb varakozas */
}

int main(int argc, char *argv[]) {
    /* ablak létrehozása */
    SDL_Window *window;
    SDL_Renderer *renderer;
    Labirintus palya;
    Koordinata player;
    palya=beolvas("palya.txt");
    player=jatekos(palya);
    int eredetikincs=kincsek(palya);
    //char str[20];
    int jelenlegi=kincsek(palya);
    //sprintf(str, "Felvett kincsek: %d", jelenlegi);
    printf("palya magas: %d es palya szeles: %d\n", palya.magassag, palya.szelesseg);
    int alap=800/palya.magassag;
    int CELLA=alap;
    sdl_init((palya.szelesseg-1)*CELLA, palya.magassag*CELLA, &window, &renderer);
    int WINDOWSZELESSEG=palya.szelesseg*CELLA;
    int WINDOWMAGASSAG=palya.magassag*CELLA;
    SDL_TimerID id = SDL_AddTimer(60, idozit, NULL);

    for(int i=0; i<=WINDOWMAGASSAG; i+=CELLA)
        //lineRGBA(kép, x1, y1, x2, y2, r, g, b, a) – szakasz.
        lineRGBA(renderer, 0, i, WINDOWSZELESSEG, i, 255, 255, 255, 155);
    for(int i=0; i<=WINDOWSZELESSEG; i+=CELLA)
        lineRGBA(renderer, i, 0, i, WINDOWMAGASSAG, 255, 255, 255, 155);
    /* rajzok */
    int x, y, r;
    r = 50;

    SDL_Texture *babuk = IMG_LoadTexture(renderer, "karakter.png");
    if (babuk == NULL) {
        SDL_Log("Nem nyithato meg a kepfajl: %s", IMG_GetError());
        exit(1);
    }
    SDL_Texture *soveny=IMG_LoadTexture(renderer, "soveny.jpg");
    SDL_Texture *coin=IMG_LoadTexture(renderer, "coin.jpg");
    SDL_Texture *finish=IMG_LoadTexture(renderer, "flag.jpg");
    if (finish == NULL) {
        SDL_Log("Nem nyithato meg a kepfajl: %s", IMG_GetError());
        exit(1);
    }
    kiir(palya);

    //filledCircleRGBA(renderer, player.oszlop*CELLA+CELLA/2, player.sor*CELLA+CELLA/2, 30, 255, 255, 255, 255);
    //palyakirajzol(player, &renderer);
    for(int i=0; i<palya.magassag; i++){
        for(int j=0; j<palya.szelesseg; j++){
            if(palya.palya[i][j]=='#'){
                babu_rajzol(renderer, soveny, j, i);
            }else if(palya.palya[i][j]=='$')
                //filledCircleRGBA(renderer, j*CELLA+CELLA/2, i*CELLA+CELLA/2, 20, 255,215,0, 255);
                babu_rajzol(renderer, coin, j, i);
            else if(palya.palya[i][j]==' ' && (i==palya.magassag-1 || j==palya.szelesseg-1 || i==0 || j==0)){
                babu_rajzol(renderer, finish, j, i);
            }
        }
    }
    babu_rajzol(renderer, babuk, player.oszlop, player.sor);
    //babu_rajzol(renderer, babuk, p, 250);

    /* szoveg */
    stringRGBA(renderer, WINDOWSZELESSEG/3-10, WINDOWMAGASSAG-CELLA/2, "Vedd fel az osszes aranyat, es juss ki!", 255, 255, 255, 255);

    /* az elvegzett rajzolasok a kepernyore */
    SDL_RenderPresent(renderer);

    /* varunk a kilepesre */
    bool quit=false;
    while (!quit) {
        SDL_Event event;
        SDL_WaitEvent(&event);

        switch (event.type) {
            /* felhasznaloi esemeny: ilyeneket general az idozito fuggveny */
            Irany merre;
            Eredmeny mezo;
            case SDL_KEYDOWN:
                printf("beleleptunk pedig!\n");
                switch (event.key.keysym.sym) {
                    printf("igen!\n");
                    case SDLK_LEFT: merre = balra; printf("balos!\n"); break;
                    case SDLK_RIGHT: merre = jobbra; break;
                    case SDLK_UP: merre = fel; printf("felfele!\n"); break;
                    case SDLK_DOWN: merre = le; break;
                    printf("ez is!\n");
                }
                if(mehete(palya, merre)){
                    //filledCircleRGBA(renderer, player.oszlop*CELLA+CELLA/2, player.sor*CELLA+CELLA/2, 30, 0, 0, 0, 255);
                    boxRGBA(renderer, player.oszlop*CELLA+1, player.sor*CELLA+1, player.oszlop*CELLA+CELLA-1, player.sor*CELLA+CELLA-1, 0, 0, 0, 255);
                    mezo=lepes(&palya, merre, &player);
                    //filledCircleRGBA(renderer, player.oszlop*CELLA+CELLA/2, player.sor*CELLA+CELLA/2, 30, 255, 255, 255, 255);
                    babu_rajzol(renderer, babuk, player.oszlop, player.sor);
                    //palyakirajzol(player, &renderer);
                    for(int i=0; i<palya.magassag; i++){
                        for(int j=0; j<palya.szelesseg; j++){
                            if(palya.palya[i][j]=='#' || palya.palya[i][j]=='|' || palya.palya[i][j]=='-'){
                                babu_rajzol(renderer, soveny, j, i);
                            }else if(palya.palya[i][j]=='$')
                                //filledCircleRGBA(renderer, j*CELLA+CELLA/2, i*CELLA+CELLA/2, 20, 255,215,0, 255);
                                babu_rajzol(renderer, coin, j, i);
                        }
                    }
                    kiir(palya);
                    SDL_RenderPresent(renderer);
                    if(mezo==kijarat)
                        quit=true;
                }
                break;

            case SDL_QUIT:
                quit = true;
                break;
        }
        //SDL_RenderPresent(renderer);
    }

    TTF_Init();
    TTF_Font *font = TTF_OpenFont("LiberationSerif-Regular.ttf", 32);
    if (!font) {
        SDL_Log("Nem sikerult megnyitni a fontot! %s\n", TTF_GetError());
        exit(1);
    }

    /* felirat megrajzolasa, kulonfele verziokban */
    SDL_Surface *felirat;
    SDL_Texture *felirat_t;
    SDL_Rect hova = { 0, 0, 0, 0 };
    SDL_Color sarga = {255, 255, 255};
    SDL_Color piros = {253, 220, 88};
    /* ha sajat kodban hasznalod, csinalj belole fuggvenyt! */
    char str[20];
    jelenlegi=kincsek(palya);
    int i=eredetikincs-jelenlegi;
    sprintf(str, "Felvett kincsek szama: %d", i);
    //felirat = TTF_RenderUTF8_Solid(font, str, feher);
    //felirat = TTF_RenderUTF8_Blended(font, str, piros);
    felirat = TTF_RenderUTF8_Shaded(font, str, sarga, piros);

    felirat_t = SDL_CreateTextureFromSurface(renderer, felirat);
    hova.x = (WINDOWSZELESSEG - felirat->w) / 2-25;
    hova.y = WINDOWMAGASSAG-75;
    hova.w = felirat->w;
    hova.h = felirat->h;
    SDL_RenderCopy(renderer, felirat_t, NULL, &hova);
    babu_rajzol(renderer, babuk, player.oszlop, player.sor);

    SDL_RenderPresent(renderer);

    SDL_FreeSurface(felirat);
    SDL_DestroyTexture(felirat_t);
    delay(3);
    /* ablak bezarasa */
    SDL_Quit();

    return 0;
}
