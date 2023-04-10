#include "lost_in_space.h"
#include "splashkit.h"

#define MIN_POWER_UP_RANGE -1500
#define MAX_POWER_UP_RANGE 1500
#define MAX_GAME_RANGE 3000

// Creating a new game
game_data new_game()
{
    game_data game;

    game.player = new_player();

    return game;
}


// Applying the effect of the powerup
void apply_power_up(game_data &game, int index)
{

    if (game.power_ups[index].kind == FUEL)
    {

        // Increasing the value only if the percentage is less than 100
        if (game.player.fuel_pct <= 1)
            game.player.fuel_pct += 0.25;

        play_sound_effect("fuel_sound");
    }
    
    else if (game.power_ups[index].kind == HEART)
    {
        // Increasing the value only if the percentage is less than 100
        if (game.player.health_pct <= 1)
            game.player.health_pct += 0.25;

        play_sound_effect("health_sound");
    }

    else
    {
        play_sound_effect("coins_sound");
    }
}

// Removing the powerup at the index from the vector
void remove_power_up(game_data &game, int index)
{
    int last_idx = game.power_ups.size() - 1;
    game.power_ups[index] = game.power_ups[last_idx];
    game.power_ups.pop_back();
}

void check_collisions(game_data &game)
{

    for (int i = game.power_ups.size() - 1; i >= 0; i--)
    {
        if (sprite_collision(game.player.player_sprite, game.power_ups[i].power_up_sprite))
        {
            // Increasing the score everytime a powerup is collected
            game.player.score++;

            // First applying the effect of the powerup and then removing it from the vector
            apply_power_up(game, i);
            remove_power_up(game, i);
        }
    }
}

void add_power_up(game_data &game)
{
    int value = rnd(MIN_POWER_UP_RANGE, MAX_POWER_UP_RANGE);

    // Adding the powerup to the vector at any random position between the paying area
    game.power_ups.push_back(new_power_up(value, value));
}

void check_power_up_position(game_data &game)
{
    for (int i = 0; i < game.power_ups.size(); i++)
    {
        double power_up_x = sprite_x(game.power_ups[i].power_up_sprite);
        double power_up_y = sprite_y(game.power_ups[i].power_up_sprite);

        // The powerup is removd if it goes out of the playing area of 3000
        if (power_up_x > MAX_POWER_UP_RANGE or power_up_y > MAX_POWER_UP_RANGE or power_up_x < MIN_POWER_UP_RANGE or power_up_y < MIN_POWER_UP_RANGE)
        {
            remove_power_up(game, i);
        }
    }

    
}


void update_game(game_data &game)
{

    // Adding a powerup in 2% of the time
    if (rnd() < 0.02)
    {
        add_power_up(game);
    }

    // Checking the collison between player and powerups
    check_collisions(game);

    // Perform movement and update the camera
    update_player(game.player);

    // Updating the powerups and checking their position
    for (int i = 0; i < game.power_ups.size(); i++)
    {
        check_power_up_position(game);

        update_power_up(game.power_ups[i]);

    }
    
}

drawing_options option_part(double x, double y, double width, double height)
{
    drawing_options options;

    options = option_part_bmp(x, y, width, height, option_to_screen());

    return options;
}

// Procedure to include the elements in the Heads up display
void draw_hud(const game_data &game)
{
    clear_screen(COLOR_BLACK);
    // Drawing the area of the HUD
    draw_rectangle(COLOR_LIGHT_BLUE, 5, 5, 520, 140, option_part(0, 0, 400, 200));

    // Drawing score and location
    draw_text("SCORE: " + to_string(game.player.score), COLOR_WHITE, 10, 20, option_to_screen());
    draw_text("LOCATION: " + point_to_string(center_point(game.player.player_sprite)), COLOR_WHITE, 10, 35, option_to_screen());

    // Drawing the fuel bar
    draw_text("FUEL: ", COLOR_WHITE, 10, 55, option_to_screen());
    draw_bitmap("empty", 40, 40, option_part(0, 0, bitmap_width("empty"), bitmap_height("empty")));
    draw_bitmap("full", 40, 40, option_part(0, 0, game.player.fuel_pct * bitmap_width("full"), bitmap_height("full")));

    // Drawing the health bar
    draw_text("HEALTH: ", COLOR_WHITE, 10, 90, option_to_screen());
    draw_bitmap("empty", 40, 75, option_part(0, 0, bitmap_width("empty"), bitmap_height("empty")));
    draw_bitmap("red_bar", 40, 75, option_part(0, 0, game.player.health_pct * bitmap_width("red_bar"), bitmap_height("red_bar")));

}

void draw_game(const game_data &game)
{
    // Redrawing everything

    // Displaying the HUD
    draw_hud(game);
    
    // as well as the player who can move
    draw_player(game.player);

    // Drawing the power ups
    for (int i = 0; i < game.power_ups.size(); i++)
    {
        draw_power_up(game.power_ups[i]);
    }

    refresh_screen(60);
}