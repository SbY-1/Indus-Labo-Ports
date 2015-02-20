BINDIR   = bin
LIB      = lib
SRC      = src
OBJ      = obj

BINS = $(BINDIR)/Gestion $(BINDIR)/Port $(BINDIR)/Dock $(BINDIR)/Boat $(BINDIR)/GenVehicle

OBJRESS = $(OBJ)/Ressources.o
OBJGEST = $(OBJ)/Gestion.o
OBJPORT = $(OBJ)/Port.o
OBJDOCK = $(OBJ)/Dock.o
OBJBOAT = $(OBJ)/Boat.o
OBJVEHI = $(OBJ)/GenVehicle.o
OBJS = $(OBJGEST) $(OBJPORT) $(OBJDOCK) $(OBJBOAT) $(OBJVEHI)

CFLAGS = $(INCLUDES)
INCLUDES = -I $(LIB)
LDFLAGS = -lrt -pthread
CC = gcc
LD = gcc

all: $(BINS)

$(OBJ)/Ressources.o: $(SRC)/Ressources.c
	@echo ">> Creating $@"
	$(CC) $(CFLAGS) -c $< -o $@

$(BINDIR)/Gestion:	$(OBJGEST)
	@echo "=> Creating $@"
	$(LD) $(CFLAGS) $^ $(LIB)/Ressources.h $(SRC)/Ressources.c $(LIB)/Common.h -o $@ $(LDFLAGS)

$(BINDIR)/Port:	$(OBJPORT)
	@echo "=> Creating $@"
	$(LD) $(CFLAGS) $^ $(LIB)/Ressources.h $(SRC)/Ressources.c $(LIB)/Common.h -o $@ $(LDFLAGS)

$(BINDIR)/Dock: $(OBJDOCK)
	@echo "=> Creating $@"
	$(LD) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(BINDIR)/Boat: $(OBJBOAT)
	@echo "=> Creating $@"
	$(LD) $(CFLAGS) $^ $(LIB)/Ressources.h $(SRC)/Ressources.c $(LIB)/Common.h -o $@ $(LDFLAGS)

$(BINDIR)/GenVehicle: $(OBJVEHI)
	@echo "=> Creating $@"
	$(LD) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ)/Gestion.o: $(SRC)/Gestion.c
	@echo ">> Creating $@"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/Port.o: $(SRC)/Port.c
	@echo ">> Creating $@"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/Dock.o: $(SRC)/Dock.c
	@echo ">> Creating $@"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/Boat.o: $(SRC)/Boat.c
	@echo ">> Creating $@"
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)/GenVehicle.o: $(SRC)/GenVehicle.c
	@echo ">> Creating $@"
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) core
