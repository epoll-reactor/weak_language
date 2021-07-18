## Simple interpreted language

Example syntax:
```
fun hello_world() {
  println("Hello, World!");
}

fun power(num, stage) {
  result = 1;
  while (stage > 0) {
    result *= num;
    --stage;
  }
  result;
}

fun main() {
  hello_world();
  println(power(2, 10));
}
```
