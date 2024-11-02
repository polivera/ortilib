# Global Makefile
all:
	$(MAKE) -C libortiarena

test:
	$(MAKE) -C libortiarena test

clean:
	$(MAKE) -C libortiarena clean
