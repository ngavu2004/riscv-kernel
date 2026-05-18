// declare prototype for uart_putstr function
void uart_putstr(const char*);

void test() {
    // Print string
    uart_putstr("Hello tiramisu!\n");

    while(1) {}
}