# Pengo
A very simple C based language written in C++ <br>
A small hobby project written in ~10 days <br>

The language is purely interpreted and parsed with an LALR(1) parser

## How to use
Run any file by running the .exe with the file name as an argument <br><br>
In Windows PowerShell:
```
./Pengo.exe fileName.pgo
```
You can also add a `-d` at the end to show the parse tree of the file
```
./Pengo.exe fileName.pgo -d
```
## Features
 - Math order of operations evaluation
 - Variables
 - Functions
 - Built in language functions
 - If, Elif, and Else conditional statements
 - While loops
 - Scoping
## Examples

## Hello World
```
println("Hello World!");
```
Output:
```
Hello World!
```

## Input
```
print("Enter a number: ");
number = int(input());
println("You entered "+number+"!");
```
Output:
```
Enter a number: 3
You entered 3!
```

## Functions
```
func add(left, right)
{
  return left + right;
}
println("3 + 4 = "+add(3,4));
```
Output:
```
3 + 4 = 7
```

## Counter
```
counter = 0;
while(counter <= 10)
{
  println(counter);
  counter = counter + 1;
}
```
Output:
```
0
1
2
3
4
5
6
7
8
9
10
```

## Conditional
```
println("What is the password?");
in = input();
if(in == "123")
  println("Access Granted!");
else
  println("Access Denied!");
```
Output:
```
What is the password?
123
Access Granted!
```

## Function Variables
```
// Sets variable equal to a built in function
p = println;
p("This prints!");

func math(operation)
{
  func add(left, right)
  {
    return left + right;
  }
  func subtract(left, right)
  {
    return left - right;
  }
  if(operation == "add")
    return add;
  elif(operation == "subtract")
    return subtract;
}

result = math("add")(4, 2);
println("Result is "+result);

result = math("subtract")(4, 2);
println("Result is "+result);
```
Output:
```
This prints!
Result is 6
Result is 2
```
