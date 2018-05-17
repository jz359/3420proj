import serial
import os
import pygame
import ast

X, Y = 1300, 1100
PADDING, TEXT_PADDING = 50, 10
BOARD_SIZE, SCALE = 1000, 0.5
WHITE = (255, 255, 255)

# calculates the point from the C rep to the pygame rep
def calc_point(point):
    return int(point * SCALE) + PADDING

# draws text on screen
def draw_text(screen, s, pos, font):
    textsurface = font.render(s, False, (0, 0, 0))
    screen.blit(textsurface,(X - 100 - PADDING*2 + TEXT_PADDING, PADDING + TEXT_PADDING * pos))

def main():
    ser = serial.Serial('COM3', 115200, timeout=2)
    pygame.init()
    pygame.font.init()
    text_font = pygame.font.SysFont('Arial', 15)
    announce_font = pygame.font.SysFont('Arial', 60)
    pygame.display.set_caption("Waypoint Catchers")
    screen = pygame.display.set_mode((X,Y))
    triangle = pygame.image.load("plane.png")
    
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                os._exit(0)
        line = ser.readline()
        if line == "DONE" or line == "CRASH": # termination conditions
            text = "YOU LOST" if line == "CRASH" else "YOU WIN!"
            textsurface = announce_font.render(text, False, (0, 0, 0))
            screen.blit(textsurface,(X/2 + PADDING, Y/2 + PADDING))
        else:
            try:
                data = ast.literal_eval(line) # data will be contained in this dict
                screen.fill(WHITE)  
                timer_data = data['time']
                
                image = pygame.transform.scale(triangle, (30,30))
                image = pygame.transform.rotate(image, 360 - int(data['heading']))
                image_rect = image.get_rect(center=screen.get_rect().center)
                screen.blit(image, (calc_point(data['x']) - image_rect.width/2,calc_point(data['y'])- image_rect.height/2)) # draw the player

                waypoint_data = data['nearest_wp']
                pygame.draw.circle(screen, (0,0,0), [calc_point(waypoint_data['x']), calc_point(waypoint_data['y'])], int(data['wp_r'] * SCALE**2) , 1) # waypoint
                
                pygame.draw.rect(screen, (0,0,0), [X - 100 - PADDING*2, PADDING, 100 + PADDING, Y - PADDING*2] , 1) # the stats panel
                pygame.draw.rect(screen, (0,0,0), [PADDING, PADDING, BOARD_SIZE, BOARD_SIZE] , 1) # the board

                # draw all the textual information
                draw_text(screen, 'X:' + str(int(data['x'])), 0, text_font)
                draw_text(screen, 'Y:' + str(int(data['y'])), 2, text_font)
                draw_text(screen, 'Z:' + str(int(data['z'])), 4, text_font)
                draw_text(screen, 'Heading:' + str(360 - int(data['heading'])), 8, text_font)
                draw_text(screen, 'Speed:' + str(int(data['v'])), 10, text_font)
                seconds_display = str(timer_data['s']) if timer_data['s'] >= 10 else "0" + str(timer_data['s'])
                draw_text(screen, 'Time:' + str(timer_data['m']) + ":" + seconds_display, 14, text_font)
                draw_text(screen, 'Waypoints:' + str(data['wp_hit']) + "/10", 16, text_font)
                if data['v'] < 70:
                    draw_text(screen, "STALL WARNING!", 18, text_font)
            except:
                continue   
        pygame.display.flip()
main()