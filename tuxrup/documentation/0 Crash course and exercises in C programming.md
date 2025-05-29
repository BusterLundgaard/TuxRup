# Kompiler og kør et basic C program:

Lav en fil kaldt `main.c` med følgende kode:

´
int main() {
    return 0;
}
´

Det her er nok det simpleste mulige c++ program. I kan bygge programmet ved at køre

`g++ main.c -o mit_mega_seje_program`

Læg mærke til at filer på linux ikke rigtig behøver at have en file extension! Især for eksekverbare filer er det tit standarden at vi ved de er eksekverbare netop fordi de *ikke* har en extension! 

Når i kører denne kommando burde i få en fil `mit_mega_seje_program` i samme mappe. I kan køre den ved bare at skrive:

`./mit_mega_seje_program`

Man kører altså et program på linux ved bare at give en path til filen. Vi er nødt til at have "./" med, fordi ellers tror den vi prøver at køre en kommando kaldt "mit_mega_seje_program". 


# Hello World:
Her skriver vi bare:

´
#include <stdio.h>
int main() {
    printf("Hello world\n");
}
´


# Input fra terminalen:
Sig at jeg gerne vil lave et program hvor jeg kan kalde programmet med

`./main buster albert`

og så printer den "Hello, buster, my name is albert.". Hvis vi skal have input fra terminalen så skal vores `main` funktion tage to parametre:

´
#include <stdio.h>
int main(int argc, char* argv[]) {
    // noget kode her
    return 0;
}
´

Det ene argument fortæller den hvor mange argumenter den skal forvente, det andet argument ... well, det er lidt teknisk. En `char` i C er én byte, så nok bits til at kode én ASCII karakter, hence, "char". I C kan man gøre noget til et array ved at sætte [] efter *navnet på variablen* (det er vildt forvirrende og dumt), så typen på `argv` er lige nu `char*[]`, så et array af `char*`. Hvad er `char*` så? En pointer til en `char`. Så vi fortæller bassicly C: "Hey C, adresen i memory på den første karakter af det første parameter til programmet starter her, adressen på første karakter af andet parameter starter her, osv". 

Vi kan nu bruge den her input som følgende:

´
#include <stdio.h>
int main(int argc, char* argv[]) {
    printf("Hello %s, my name is %s, argv[1], argv[2]);
    return 0;
}

´
Her bruger vi igen `printf`, som kan tage en formateret streng og nogle inputs på dele af strengen den skal erstatte. Så det virker lidt ligesom i måske kender det fra Go. Faktisk så kan i bare tænke på Go hvis i skal forstå hvordan mange ting virker i C, da de to sprog på mange måder minder om hinanden. 

Bemærk at vi starter parametrene fra index 1, ikke 0. Det gælder ikke arrays generelt i C, det er bare lige i dette ene tilfælde. Weird stuff.

# Basic opgave 1:
Brug funktionen `atoi` fra `stdlib.h` (som i kan inkludere på samme måde som `stdio.h` fra før) til at tage et tal fra brugeren, og så printe 2 gange det tal. 
I får nok brug for at researche selv hvordan man printer en `int` med `printf`. 


# Typer og størrelser i C:
"Typer" i C er vitterligt bare størrelser på hvor stor C forventer en variabel er

`
#include <stdio.h>

int main() {
    char a = 10;
    short b = 10;
    int c = 10;
    long int d = 10;
    long long int e = 10;

    printf("Size of char is %lld\n", sizeof(a));
    printf("Size of short is %lld\n", sizeof(b));
    printf("Size of int is %lld\n", sizeof(c));
    printf("Size of long int is %lld\n", sizeof(d));
    printf("Size of long long int is %lld\n", sizeof(e));

    return 0;
}
` 

Compiler og kør dette program, og i kan se hvor mange bytes hver af de her primitive typer er. 

## Basic opgave 2:
Hvor mange bytes er en float?

## Basic opgave 3:
Lav en funktion der tager en `int` som input, og printer bitsne på den int, altså, den binære form af tallet. 


# Structs i C:
C understøtter ikke klasser, men man kan åbne rigtig meget med structs! Structs er bare et sæt af typer i en bestemt rækkefølge som man giver nogle navne, f.eks:

´
struct Person {
    int age;
    float weight;
    long long int social_security_number;
};

int main() {
    struct Person my_person = {99, 120.5, "Kurt"};
    printf("I am %d years old, i weigh %f kgs, and my name is %s", my_person.age, my_person.weight, my_person.name);
}
´
Præcis ligesom du kender i andre sprog så bruger du punktum til at access fields. Så en struct er mere eller mindre bare en klasse, minus metoderne - kun dataen. Og så er alle felterne altid public. 

## Basic opgave 4:
Hvad forventer du at `sizeof(Person)` giver? Test dit svar. 


# Macro'er:
Fordi der er så få indbyggede funktioner i C, så ender folk tit med at lave "makroer" for at udvide funktionaliteten en smule, eller gøre noget lidt mere praktisk at skrive. F.eks. sig at vi gerne ville skrive en `max(a, b)` funktion der to det største af to parametre. C understøtter ikke operator overloading, så vi ville være nødt til at skrive en seperat funktion for floats, int, long ints, ects. Vi kunne i stedet vælge at skrive en makro:
´
#define MAX(a, b) a > b ? a : b

int main() {
    int a1 = 23;
    float a2 = 50;
    long long int a3 = 588949029043;

    printf("%f \n", MAX(a1, a2));
    printf("%lld \n", MAX(a1, a3));
}
´

Markoen tager vitterligt bare enhver instans af MAX(a, b) og udvider det til a > b ? a : b. Den copy paster altså bare tekst, og er fuldkommen ligeglad med om det er sikkert at gøre overhovedet. Makroer er derfor tit ret risky business, og hvis man kan undgå det bør man undgå det. Men vi kommer 100% til at støde på makroer. Sådan er det bare. 

## Basic opgave 5:
C understøtter ikke bools direkte. Den ser bare `0` som false og et hvilket som helst tal der ikke er `0` som true. Typen på bools er ofte `char`, da det bare er den mindste størrelse et variabel kan have i C (1 byte). 

Definer makroer så vi kan arbejde med bools på samme måde som i andre sprog. 


# Pointers:
Sådan! Nu skal vi ordentlig i gang med C programmering og snakke om pointers. Hvis en variabel i C har typen `type*` så repræsenterer den variabel en pointer til en variabel af typen `type`. Så `int*` er en pointer til en `int` (og vi forventer derfor at læse 4 bytes fra den adresse), `long long int*` er en pointer til en `long long int` (og vi forventer derfor at læse 8 bytes derfra), og `Person**` er en pointer til en pointer til en struct Person, og alle pointers har en størrelse på 8, så vi forventer igen at læse 8 bytes. 

Syntaksen vi bruger til pointers i C er meget mærkelig. Generelt set: "&" tager adressen af noget, "*" laver noget om fra en adresse til værdien på den adresse, og "->" er ligesom dot-operatoren på structs, men hvis du har en pointer til en struct i stedet. Lad os se på et par eksempler:

´
#include <stdio.h>

struct Person {
    int age;
    float weight;
    char* name;
};

void print_person(struct Person p){
    printf("I am %d years old, i weigh %f kgs, and my name is %s\n", p.age, p.weight, p.name);
}

int main() {

    struct Person my_person = {99, 120.5, "Kurt"};

    struct Person* my_person_pointer = &my_person;
    my_person_pointer->age = 34;
    print_person(my_person);

    struct Person my_new_person = *my_person_pointer;
    my_new_person.name = "Buster";
    print_person(my_new_person);

    return 0;
}
´

Så vi definerer en Person, vi får en pointer til den person, ved hjælp af den pointer ændrer vi på et field i Person, så `age` på variablen `my_person` har ændret sig når vi derefter kalder `print_person`. 

## Basic opgave 6:
Ændre `print_person` funktionen så den ikke tager en hel person, men bare tager en pointer til en person. Det er jo fjollet at den faktisk tager en helt person, for alt er pr. default pass-by-value i C. Det vil sige vi kopiere og skaber en helt ny person bare for at kalde funktionen, hvilket vi spilder en masse memory og performance på. Det eneste `print_person` har brug for er bare en pointer til en `Person`, og den ved vi fylder kun 8 bytes. 

Så hvis du gerne vil have at noget er pass-by-refference i C, så er du altså nødt til at bruge pointers! 


# Casting af pointers:
Hvis du har en pointer til en eller anden type `type`, så kommer C jo altid til at læse værdier ved den pointer som om det var en `type`. Hvis du gerne vil sige: "Hey, faktisk læs værdien ved denne adresse som om det var en `int` i stedet!" så kan du *caste* din pointer:

´
// Denne værdi er ikke for stor til en int, så alt virker fint
long long int sus = 284983;
long long int* sus_pointer = &sus; //Denne pointer forventer nu at læse 8 bytes
int* small_sus_pointer = (int*)sus_pointer;
printf("%d \n", *small_sus_pointer);

// Denne værdi er for stor, så vi får noget random nonsens:
sus = 2849833498049832;
printf("%d \n", *small_sus_pointer);
´

Kør koden her og se hvad der sker. 

## Basic opgave 7:
Lav en funktion der tager en pointer til en float, og så printer hvad dens 4 bytes er i memory (ikke nødvendigvis hvad binær formen af denne float er, men værdien på de 4 bytes der bruges til faktisk at repræsentere den i memory)

Hint: Her får i brug for at caste til en `unsigned char*`, da vi gerne vil læse 8 bits (og en char har størrelsen 1 byte), og vi skal ikke tyde det som om tallet kunne være negativt, så det skal være `unsigned`. For at printe en unsigned char med `printf` skal man bruge formatteren `%hhu` af en eller anden grund (åbenbart "u" for unsigned og "hh" for "single bit" ¯\_(ツ)_/¯ )


# Stack og heap:
Indtil videre minder pointere i C sort of en del om pointere i Go, men hvor tingende virkelig begynder at blive anderledes er når det handler om *heap* vs *stack*. 

I Dovs kan du måske godt huske hvordan vores aller første simple X86 compiler brugte en "stack" af værdier/operands til variabler. Det er mere eller mindre det samme C gør. Alle dine lokale variabler i en funktion bliver tilføjet til stakken. Stakken er relativt lille (meget meget mindre end hele din RAM selvfølgelig), men den er drønhurtig. Lad os tage et eksempel der lidt kan fucke med hjernen:

`
#include <stdio.h>

struct Person {
    int age;
    float weight;
    char* name;
};

struct Person* make_person_and_give_pointer(){
    struct Person p = {20, 30.5, "dingelingdong"};
    return &p;
}

int main() {
    struct Person* p_pointer = make_person_and_give_pointer();
    printf("My age is: %d", p_pointer->age);

    return 0;
}
`

Nej, dette program printer ikke "My age is: 20", det crasher og og giver "Segmentation fault (core dumped)". Den kommer i til at se en del når i arbejder i C. Din `Person` p du laver i funktionen lever på stacken. Når du er færdig med at kalde funktionen så fjernes den fra stakken igen. Så når pointeren returens og faktisk lægges ind i `p_pointer`, så peger den på noget der slet ikke findes længere! Så vi prøver at retrieve en værdi til et sted i memory vi ikke har rettighed til længere, så vores process bliver skudt af operativsystemet. Hence, segmentation fault. 

Lad os lige gøre alting lidt simplere for nu og fjerne navnet fra vores `Person` som jo er en string (og vi har ikke helt diskuteret endnu hvordan de virker ... ). Hvis vi skulle fikse `make_person_and_give_pointer()` så skulle vi i stedet sige:

`
#include <stdio.h>

struct Person {
    int age;
    float weight;
};

struct Person* make_person_and_give_pointer(){
    struct Person* p = (struct Person*)malloc(sizeof(struct Person));
    p->age = 20;
    p->weight = 30.5;
    return p;
}

int main() {
    struct Person* p_pointer = make_person_and_give_pointer();
    printf("My age is: %d \n", p_pointer->age);

    return 0;
}
`

`malloc(n)` er en meget special og vigtig kommando/funktion. Den spørger operativsystemet om lov til at få `n` ekstra bytes memory af til dens process, og så returnerer den pointeren til det sted i hukommelsen den har fået lov til at bruge. `malloc` i sig selv returner en såkaldt `void*`, hvilket bare er en helt generisk pointer til memory, uden du rigtig ved hvad der ligger der / hvor mange bytes du skal læse. Vi caster den void pointer til en `Person` pointer, og skriver så manuelt værdierne ind i memory. Nu lever vores person ikke på stacken, den leaver **i memory**! Den lever **i vores ram**, hvilket er det vi i C-terminologi også kalder *heap*. Mere eller mindre så gør vi det her så manuelt og primitivt man kunne forestille sig: For en adresse i ram og manuelt sætter bitsne fra den adresse. Bum! 

## Basic opgave 8:
Undersøg hvordan funktionen `strcpy` i C virker, og udvid `make_person_and_give_pointer` så den også virker hvis vores `Person` struct stadig har en `char* name` field. 

## Endelig test af din C forståelse (lidt et difficulty spike):
Implementer en linked list af ints i C. Din liste bør understøtte `get(index)`, `add(index, value)` og `remove(index)` operationer. Held og lykke, det her fucker seriøst med ens hjerne på et helt nyt niveau, men det er ret vigtigt for ordentligt at forstå hvordan heap vs stack fungerer. 

