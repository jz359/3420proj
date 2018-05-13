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
    pygame.display.set_caption("waypoint catcher")
    
    screen = pygame.display.set_mode((X,Y))


    # main loop
    while True:
        # event handling, gets all event from the eventqueue
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                os._exit(0)
        line = ser.readline()
        try:
            data = ast.literal_eval(line)
        except:
            continue
        screen.fill(WHITE)  
        
        pygame.draw.circle(screen, RED, [calc_point(data['x']),calc_point(data['y'])], 15, 0)

        waypoint_data = data['nearest_waypoint']
        pygame.draw.circle(screen, (0,0,0), [calc_point(waypoint_data['x']), calc_point(waypoint_data['y'])], int(200 * SCALE) , 1)
        
        pygame.draw.rect(screen, (0,0,0), [X - 100 - PADDING*2, PADDING, 100 + PADDING, Y - PADDING*2] , 1)
        pygame.draw.rect(screen, (0,0,0), [PADDING, PADDING, BOARD_SIZE, BOARD_SIZE] , 1)

        textsurface = text_font.render('X:' + str(int(data['x'])), False, (0, 0, 0))
        screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING))
        textsurface = text_font.render('Y:' + str(int(data['y'])), False, (0, 0, 0))
        screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING*2))
        textsurface = text_font.render('Z:' + str(int(data['z'])), False, (0, 0, 0))
        screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING*4))
        textsurface = text_font.render('Heading:' + str(int(data['heading'])), False, (0, 0, 0))
        screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING*6))
        pygame.display.flip()
     
# run the main function only if this module is executed as the main script
# (if you import this as a module then nothing is executed)
if __name__=="__main__":
    # call the main function    
    main()