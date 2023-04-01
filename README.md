# jvar
JVAR is a dynamic multi type variable similar to Javascript variables, it brings flexibility into a typed language.

# Building this project
In a linux machine (or windows wsl) with build essentials installed, it is as simple as writing: make  ,in the command line.
sudo apt-get install build-essential
''' make
There is run.sh as a utility that will kill a last project instance running, compile and spin up a new instance.

#Using the lib
JVAR is contained in jvar.cpp and jvar.h, jvar.h is commented with explanations of every functionality. Right now it only depends on picojson.h(for parsing json).
To use in your project just copy jvar.cpp, jvar.h, picojson.h and include jvar.h to one of your includes.

JVAR

Jvar é uma das crianças dos olhos da Brasili, é uma classe em C++ com o proposito de facilitar a entrada e saída de dados nos microserviços em c++.

Jvar é uma variavel capaz de representar qualquer um dos seguintes tipos:

string, numérico, booleano, array, objeto e mnull.

mnull é uma classe vazia com o propósito de indicar que aquela variável não contém valor algum.

Basicamente se adiciona a linguagem fortemente tipada um tipo variável como  é o padrão em linguagens dinâmicas como Python, Javascript, PHP.

Jvar se integra naturalmente com os tipos de c++ e esse é seu grande poder. Além disso apresenta utilidades de tipos baseados em javascript como substr, replace,indexOf,split,join.

Por exemplo se tivermos a seguinte função:

fazerAlgo(int a1, std::string b2, float c3);


Poderíamos passar jvar para os três parâmetros.

jvar a = 1, b = “potato”, c = 0.5;


e chamar

fazerAlgo(a, b, c);


Isso é especialmente útil quando estamos recebendo dados de outros micro-serviços e do banco de dados, com esse tipo flexível e fácil de usar, não é necessário definir os tipos de dados em todos microserviços, apenas é necessário definir onde o dado é originado.

Exemplos de uso:

jvar jva = 1;
std::string b = "batata";

jvar c = b + jva; //output "batata1"

int  d =55;
double e = 23.5;

jvar f = jva + d + e; //output 79.5
int g = jva;
double g = jva;
std::string h = jva;


Objetos

"jo" é jvar object resumidamente, e representa uma inicialização com um std::unordered_map<std::string,jvar>

"jv" é um (jvar) resumidamente, um cast de tipo para facilitar o operador <<

o operador "string" << jvar é usado para gerar uma entrada chave:valor em um jo, note que jo é meramente um inicializador, o resultado final é uma jvar.

jvar i = jo({
"a" << jv 1,
"b" << jv "potato",
"c" << jv false,
"d" << jv 0.5,
"e" << jv MNULL
});
//também pode inicializar diretamente com valores:
jvar j;
j["a"] = 1;
j["b"] = "potato";
j["c"] = false;
j["d"] = 0.5;
j["e"] = MNULL;
j["myarray"] = ja({1,2,"a",2.5,MNULL});


Arrays

"ja" é jvar array resumidamente e representa uma inicialização com std::vector<jvar>

jvar k = ja({1,false,true,MNULL,"potato",0.5});

//também pode inicializar diretamente com valores:

k[ZERO] = 1;
k[1] = 2.3;
k[2] = "potato";
k[3] = false;
k[4] = MNULL;
k[5] = jo({ "a" <<jv "object because we can" , "yup" << jv true});


Infelizmente em C++ o valor 0 pode representar um caractere vazio, e portanto é ambiguo no operador [] da jvar que aceita caracteres e numeros, por isso definimos "ZERO" como (size_t)0 e usamos ao invés.

foreach 

Funciona com todos os tipos de jvar. Não é necessário tratar se houver um ou mais itens diferentemente, tudo pode ser tratado no plural

for(jvar& item : meuJvar) // ou j, ou i, ou f ou jva, etc
{
 std::cout << item << std::endl;
 //do something, works for all jvar types
}


Utilidades

jvar parseJson(std::string json);


Conversores 'óbvios'

bool asBoolean();
double asDouble();
long long int asInteger();
std::string toJson();
std::string prettyString(); //pretty json for debugging
std::string asString();


Checadores de Tipo

bool isNumeric();
bool isInteger();
bool isRealNumber();
bool isString();
bool isArray();
bool isObject();


Indicadores de tamanho

size_t size(); //mostra a quantidade de elementos (string, object, array ou 0)
size_t memoryFootPrint(); //mostra quanta memoria o objeto está usando
bool isEmpty(); //true se size for 0


Utilidades de objeto ou array

No caso de uso com outros tipos retornará ainda valores na mesma forma, sempre com 1 elemento

jvar entries(); //retorna j array com forma [ {"chave":valor}, ... ]
// [{"0":valor}] para outros tipos
 e [{"0":valor,"1":valor,...}] para array
jvar keys(); //retorna j array com forma [ "chave1","chave2",... ]
jvar values(); //retorna j array com forma [ "valor1",2,... ]


Utilidades de Array ou String

size_t indexOf(std::string str, size_t start=0);
//Retorna posição de elemento de texto em um array ou posição do elemento dentro de uma string
//std::string::npos caso contrário


Utilidades de String

jvar replaceAll( const std::string &search, const std::string &replace );
//troca todas ocorrencias de "search" com "replace"

jvar replace( const std::string &search, const std::string &replace );
//troca a primeira ocorrencia de "search" com "replace"

jvar substr(size_t start, size_t end);
//retorna um pedaço da string delimitado por start e end

jvar substr(size_t start);
//retorna um pedaço da string delimitado por start e o fim da string

//Atualmente só funciona com caracteres ASCII, há planos para abranger todo UTF-8
jvar toLowerCase();
jvar toUpperCase();
jvar toCapitalized();


jvar split(std::string separator); // transforma string em um array delimitado por separator

jvar join(std::string separator); //transforma um array em uma string unida por separator
