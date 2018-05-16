import serial
import time
import os
import pygame
import math
import ast

X = 1300
Y = 1100
PADDING = 50
TEXT_PADDING = 10

BOARD_SIZE = 1000
SCALE = 0.5;

RED = (255, 0, 0)
WHITE = (255, 255, 255)

def calc_point(point):
    return int(point * SCALE) + PADDING

# define a main function
def main():
    ser = serial.Serial('COM3', 115200, timeout=2)

    pygame.init()
    pygame.font.init()

    text_font = pygame.font.SysFont('Arial', 15)
    announce_font = pygame.font.SysFont('Arial', 60)
    pygame.display.set_caption("Waypoint Catchers")
    
    screen = pygame.display.set_mode((X,Y))

    triangle = pygame.image.load("triangle.png")
    image_rect = triangle.get_rect(center=screen.get_rect().center)
    
    # main loop
    while True:
        # event handling, gets all event from the eventqueue
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                os._exit(0)
        line = ser.readline()
        if line == "DONE" or line == "CRASH":
            text = "YOU LOST" if line == "CRASH" else "YOU WIN!"
            textsurface = announce_font.render(text, False, (0, 0, 0))
            screen.blit(textsurface,(X/2 + PADDING, Y/2 + PADDING))
        else:
            try:
                data = ast.literal_eval(line)
                # print data
                screen.fill(WHITE)  
                timer_data = data['time']
                
                # pygame.draw.circle(screen, RED, [calc_point(data['x']),calc_point(data['y'])], 13, 0)
                image = pygame.transform.rotate(triangle, 270 - int(data['heading']))
                image = pygame.transform.scale(image, (30,30))
                screen.blit(image, (calc_point(data['x'] - 15),calc_point(data['y'] - 15)))

                waypoint_data = data['nearest_wp']
                pygame.draw.circle(screen, (0,0,0), [calc_point(waypoint_data['x']), calc_point(waypoint_data['y'])], int(data['wp_r'] * SCALE**2) , 1)
                
                pygame.draw.rect(screen, (0,0,0), [X - 100 - PADDING*2, PADDING, 100 + PADDING, Y - PADDING*2] , 1)
                pygame.draw.rect(screen, (0,0,0), [PADDING, PADDING, BOARD_SIZE, BOARD_SIZE] , 1)

                # draw all the textual information
                textsurface = text_font.render('X:' + str(int(data['x'])), False, (0, 0, 0))
                screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING))
                textsurface = text_font.render('Y:' + str(int(data['y'])), False, (0, 0, 0))
                screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING*2))
                textsurface = text_font.render('Z:' + str(int(data['z'])), False, (0, 0, 0))
                screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING*4))
                textsurface = text_font.render('Heading:' + str(360 - int(data['heading'])), False, (0, 0, 0))
                screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING*8))
                textsurface = text_font.render('Speed:' + str(int(data['v'])), False, (0, 0, 0))
                screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING*10))

                if data['v'] < 70: 
                    textsurface = text_font.render("STALL WARNING!", False, (0, 0, 0))
                    screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING*18))

                seconds_display = str(timer_data['s']) if timer_data['s'] > 10 else "0" + str(timer_data['s'])
                textsurface = text_font.render('Time:' + str(timer_data['m']) + ":" + seconds_display, False, (0, 0, 0))
                screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING*14))

                textsurface = text_font.render('Waypoints:' + str(data['wp_hit']) + "/10", False, (0, 0, 0))
                screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING*16))
            except:
                continue   
        pygame.display.flip()
      
main()