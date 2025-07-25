# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile -- ft_traceroute                          :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: javi <marvin@42.fr>                        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/14 19:13:49 by javi              #+#    #+#              #
#    Updated: 2025/07/18 18:36:42 by javi             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

TARGET    = $(BINDIR)/ft_traceroute
TARGET.B  = $(BINDIR)/ft_traceroute_bonus

OBJDIR    = obj
SRCDIR    = sources
HDRDIR    = headers
SRCDIR.B  = sources/bonus
HDRDIR.B  = headers/bonus
BINDIR    = bin

HEADERS   = -I./$(HDRDIR)
SOURCES   = $(wildcard $(SRCDIR)/*.c)
OBJECTS   = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SOURCES))
DEPS      = $(OBJECTS:.o=.d)

HEADERS.B = -I./$(HDRDIR.B)
SOURCES.B = $(wildcard $(SRCDIR.B)/*.c)
OBJECTS.B = $(patsubst $(SRCDIR.B)/%.c, $(OBJDIR)/%.o, $(SOURCES.B))
DEPS.B    = $(OBJECTS.B:.o=.d)

CC        = gcc
CFLAGS    = -Wall -Wextra -Werror $(HEADERS)
CFLAGS.B  = -Wall -Wextra -Werror $(HEADERS.B)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS) -o $@ $^ -lm

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

-include $(DEPS)

bonus: $(TARGET.B) setbonus

$(TARGET.B): $(OBJECTS.B)
	@mkdir -p $(BINDIR)
	$(CC) $(CFLAGS.B) -o $@ $^ -lm

$(OBJDIR)/%.o: $(SRCDIR.B)/%.c
	@mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS.B) -MMD -MP -c $< -o $@

-include $(DEPS.B)

setcap: all
	sudo setcap cap_net_raw+ep $(TARGET)

setpost: 
	sudo setcap cap_net_raw+ep $(TARGET)

setbonus:
	sudo setcap cap_net_raw+ep $(TARGET.B)

clean:
	rm -rf $(OBJDIR)/*.o $(OBJDIR)/*.d

fclean: clean
	rm -f $(TARGET)
	rm -f $(TARGET.B)
	rm -rf $(BINDIR)

re: fclean all

.PHONY: all clean fclean re
