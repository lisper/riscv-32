
volatile int c = 1;

int main() {
	c = 1;
	for (int i = 0; i < 5; i++) {
		c <<= 1;
	}
	for (int i = 0; i < 5; i++) {
		c >>= 1;
	}
}
