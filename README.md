## Simple interpreted language

Example syntax:
```
fun hello_world() {
  println("Hello, World!");
}

fun power(num, stage) {
  result = 1;
  while (stage > 0) {
    result = result * num;
    stage = stage - 1;
  }
  result;
}

fun main() {
  hello_world();
  println(power(2, 10);
}
```
