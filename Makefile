# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: orazafy <orazafy@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/23 20:32:03 by orazafy           #+#    #+#              #
#    Updated: 2024/03/07 17:24:17 by orazafy          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++
CFLAGS = -Wall -Werror -Wextra -g -std=c++98
IFLAGS = -I$(INCDIR)

SRCDIR = srcs
OBJDIR = objs
BINDIR = bin
INCDIR = include

SRCS_FILES = main.cpp \
			ACommand.cpp \
			Channel.cpp \
			Client.cpp \
			CommandResponses.cpp \
			Configuration.cpp \
			ErrorReplies.cpp \
			CommandPrivmsg.cpp \
			CommandPass.cpp \
			CommandNick.cpp \
			CommandJoin.cpp \
			CommandUser.cpp \
			CommandMode.cpp \
			CommandInvite.cpp \
			CommandTopic.cpp \
			CommandKick.cpp \
			CommandPing.cpp \
			CommandPart.cpp \
			CommandQuit.cpp \
			Parser.cpp \
			Server.cpp \
			Utils.cpp \
			Logger.cpp

SRCS = $(addprefix $(SRCDIR)/, $(SRCS_FILES))
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

INCS_FILES = ACommand.hpp \
			Channel.hpp \
			Client.hpp \
			CommandResponses.hpp \
			Configuration.hpp \
			ErrorReplies.hpp \
			CommandPrivmsg.hpp \
			CommandPass.hpp \
			CommandJoin.hpp \
			CommandNick.hpp \
			CommandUser.hpp \
			CommandMode.hpp \
			CommandInvite.hpp \
			CommandTopic.hpp \
			CommandKick.hpp \
			CommandPing.hpp \
			CommandPart.hpp \
			CommandQuit.hpp \
			Parser.hpp \
			Server.hpp \
			Utils.hpp \
			Logger.hpp \
			ModeInfo.hpp \
			Colors.hpp

INCS = $(addprefix $(INCDIR)/, $(INCS_FILES))

NAME = $(BINDIR)/ircserv

RMR = rm -rf
MKDIR_P = mkdir -p

all: $(NAME)

$(NAME): $(OBJS) | $(BINDIR)
	$(CC) $(CFLAGS) $(OBJS) $(IFLAGS) -o $(NAME) 

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(INCS) | $(OBJDIR)
	$(CC) $(CFLAGS) $(IFLAGS) -c $< -o $@ 

$(BINDIR):
	$(MKDIR_P) $(BINDIR)

$(OBJDIR):
	$(MKDIR_P) $(OBJDIR)

clean:
		$(RMR) $(OBJDIR)

fclean: clean
		$(RMR) $(BINDIR)

re: fclean all

.PHONY: all clean fclean re
