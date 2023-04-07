# jvar
JVAR is a dynamic self managed multi type variable similar to Javascript variables, it brings flexibility into c++ typed language.

# Building this project
In a linux machine (or windows wsl) with build essentials installed, it is as simple as writing: make  ,in the command line.
`
sudo apt-get install build-essential ;
make
`
There is run.sh as a utility that will kill a last project instance running, compile and spin up a new instance.

# Using the lib
JVAR is contained in jvar.cpp and jvar.h, jvar.h is commented with explanations of every functionality. Right now it only depends on picojson.h(for parsing json).
To use in your project just copy jvar.cpp, jvar.h, picojson.h and include jvar.h to one of your includes.

# JVAR

Jvar is one of Brasili's most cherished projects, and it is a C++ class designed to facilitate input and output of data in C++ microservices.

Jvar is a variable capable of representing any of the following types: string, numeric, boolean, array, object, and mnull.

mnull is an empty class designed to indicate that the variable does not contain any value.

Basically, Jvar adds a variable type to the strongly-typed language, which is standard in dynamic languages like Python, JavaScript, and PHP.

Jvar naturally integrates with C++ types, and this is its greatest strength. Additionally, it provides JavaScript-based utility types such as substr, replace, indexOf, split, and join.

For example, if we have the following function:

doSomething(int a1, std::string b2, float c3);


We could pass Jvar for all three parameters.

jvar a = 1, b = “potato”, c = 0.5;


and call

doSomething(a, b, c);

This is especially useful when receiving data from other microservices and the database. With this flexible and easy-to-use type, it is not necessary to define data types in every microservice. It is only necessary to define where the data originated ( the golden source ).

Use Cases:

jvar jva = 1;
std::string b = "potato";

jvar c = b + jva; //output "potato1"

int  d =55;
double e = 23.5;

jvar f = jva + d + e; //output 79.5
int g = jva; //pass jvar to any typed var
double g = jva;//pass jvar to any typed var
std::string h = jva;//pass jvar to any typed var


# Objects

"jo" is a shorthand for Jvar Object and represents an initialization with a std::unordered_map<std::string, jvar>.

"jv" is a shorthand for jvar, and it is a type cast used to facilitate the << operator.

The string << jvar operator is used to generate a key-value entry in a jo. Note that jo is merely an initializer, and the final result is a jvar.

jvar i = jo({
"a" << jv 1,
"b" << jv "potato",
"c" << jv false,
"d" << jv 0.5,
"e" << jv MNULL
});
//you can also initialize values directly in the object:
jvar j;
j["a"] = 1;
j["b"] = "potato";
j["c"] = false;
j["d"] = 0.5;
j["e"] = MNULL;
j["myarray"] = ja({1,2,"a",2.5,MNULL});


# Arrays

"ja" is a shorthand for Jvar Array and represents an initialization with std::vector<jvar>.

jvar k = ja({1,false,true,MNULL,"potato",0.5});

// You can also initialize it directly with values:

k[ZERO] = 1;
k[1] = 2.3;
k[2] = "potato";
k[3] = false;
k[4] = MNULL;
k[5] = jo({ "a" <<jv "object because we can" , "yup" << jv true});


Unfortunately, in C++, the value 0 can represent an empty character, making it ambiguous in the operator [] of jvar which accepts characters and numbers. Therefore, we define "ZERO" as (size_t)0 and use it instead.

# foreach 

The "foreach" loop works with all types of jvar. There's no need to treat single or multiple items differently, everything can be treated in the plural form.

For example:
`
for (jvar& item : myvar) // or j, or i, or f or jva, etc
{
  std::cout << item << std::endl;
  // do something, works for all jvar types
}
`
In this code snippet, the "foreach" loop can be used with all types of jvar, and there's no need to treat single or multiple items differently. The loop iterates over each item in myvar, and the std::cout statement can be used to perform some action on each item, regardless of its type.

# Utilities

jvar parseJson(std::string json);

Obvious converters:

bool asBoolean();
double asDouble();
long long int asInteger();
std::string toJson();
std::string prettyString(); //pretty json for debugging
std::string asString();


Type Checkers

bool isNumeric();
bool isInteger();
bool isRealNumber();
bool isString();
bool isArray();
bool isObject();


# Size Indicators

size_t size(); //show amount of elements (string, object, array or 0)
size_t memoryFootPrint(); //show how much memory that object is using
bool isEmpty(); //true if size is 0


# Object and Array utilities

In case of use with other types, it will still return values in the same form, always with 1 element.

jvar entries(); //returns j array in the form [ {"key":value}, ... ]
// [{"0":value}] for other types
// and [{"0":value,"1":value,...}] for array
jvar keys(); //returns j array in the form [ "key1","key2",... ]
jvar values(); //returns j array in the form [ "value1",2,... ]


# Array or String Utilities

size_t indexOf(std::string str, size_t start=0);
// Returns the position of a text element in an array or the position of an element within a string
// std::string::npos otherwise


# String Utilities

jvar replaceAll(const std::string& search, const std::string& replace);
// Replaces all occurrences of "search" with "replace"

jvar replace(const std::string& search, const std::string& replace);
// Replaces the first occurrence of "search" with "replace"

jvar substr(size_t start, size_t end);
// Returns a substring delimited by start and end

jvar substr(size_t start);
// Returns a substring delimited by start and the end of the string

// Currently only works with ASCII characters, there are plans to cover all UTF-8
jvar toLowerCase();
jvar toUpperCase();
jvar toCapitalized();


jvar split(std::string separator); // turns string into an array delimited by separator

jvar join(std::string separator); // turns an array into a string joined by separator
