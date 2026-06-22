#include <random>
#include <vector>
#include <queue>
#include <limits>
#include <utility>
#include "../include/raylib.h"


using namespace std;


Font customFont ;
int max_val = numeric_limits<int>::max();


struct Node {
    int x {};
    int y {};
    int f  = max_val;
    int g  = max_val;
    int h  = 0;
    bool is_start = false;
    bool is_end = false;
    bool is_obs = false;
    bool is_closed = false;
    bool in_current_path = false;
    pair<int , int> parent_index { -1 , -1};
};

struct Compare {
    bool operator()(const Node* a, const Node* b) const {
        return a->f > b->f; 
    }
};


const int WIDTH = 800;
const int HEIGHT = 800;
const int GRID_BOX_SIZE = 20;
const int BOXES_COUNT = WIDTH / GRID_BOX_SIZE;
const int OBS = (BOXES_COUNT * BOXES_COUNT) * 0.3;  

vector<vector<Node>> m_grid(BOXES_COUNT , vector<Node>(BOXES_COUNT));
priority_queue<Node* , vector<Node*> , Compare> m_opened;




void DrawGrid(){
    for (int row = 0; row < BOXES_COUNT; row++) {
        for (int col = 0; col < BOXES_COUNT; col++) {

            int x_position = col * GRID_BOX_SIZE;
            int y_position = row * GRID_BOX_SIZE;

            Color color  = BLACK;
            if (m_grid[row][col].is_closed) color = DARKGREEN;
            if (m_grid[row][col].in_current_path) color = GOLD;
            if (m_grid[row][col].is_start) color = GREEN ;
            if (m_grid[row][col].is_end) color = RED;
            if (m_grid[row][col].is_obs) color = BLUE; 
            if (m_grid[row][col].is_start || m_grid[row][col].is_end || m_grid[row][col].is_obs || m_grid[row][col].is_closed || m_grid[row][col].in_current_path) {
                DrawRectangle(x_position , y_position , GRID_BOX_SIZE , GRID_BOX_SIZE , color);
            }
            DrawRectangleLines(x_position , y_position , GRID_BOX_SIZE , GRID_BOX_SIZE , DARKGRAY);
        }
    }
}

random_device rd; 
mt19937 gen(rd());
uniform_int_distribution<int> distrib(1  , BOXES_COUNT - 2);


void DrawOBS(){
     int current_obs = OBS;
     while (current_obs) {
         int i = distrib(gen);
         int j = distrib(gen);

         if (!m_grid[i][j].is_obs &&
             !m_grid[i][j].is_start &&
             !m_grid[i][j].is_end){
             m_grid[i][j].is_obs = true; 
             current_obs -= 1;
         }

     }
}

int heuristic(const Node& n1, const Node& n2){
    return abs(n1.x - n2.x) + abs(n1.y - n2.y);
}

bool is_valid(int x, int y) {
    return x >= 0 && x < BOXES_COUNT &&
           y >= 0 && y < BOXES_COUNT &&
           !m_grid[y][x].is_obs;
}

vector<Node*> get_neighbors(Node* node){
    vector<Node*> neighbors;

    vector<pair<int,int>> steps = {
        {0 , -1}, 
        {0 ,  1}, 
        {1 ,  0}, 
        {-1,  0}, 
    };

    for(auto& p : steps){
        int x = node->x + p.first;
        int y = node->y + p.second;

        if(is_valid(x, y)){
            neighbors.push_back(&m_grid[y][x]);
        }
    }

    return neighbors;
}


Node* start = &m_grid[0][0];
Node* goal  = &m_grid[BOXES_COUNT-1][BOXES_COUNT-1];
bool found = false;

void a_star_step(){
    if (m_opened.empty() || found) return;

     Node* node = m_opened.top();
     m_opened.pop();

     if(node->is_obs || node->is_closed)
         return;

     if(node == goal)
     {
         found = true;
         vector<Node*> path;

         pair<int,int> parent_pos = goal->parent_index;
         Node* current = goal; 
         while (current && !current->is_start){
             current = &m_grid[parent_pos.second][parent_pos.first]; 
             parent_pos = current->parent_index;
             path.push_back(current);
         }


         for(Node* n : path){
             n->in_current_path = true;
         }
     }

     node->is_closed = true;

     for(Node* n : get_neighbors(node))
     {
         if(n->is_closed || n->is_obs)
             continue;

         int new_g = node->g + 1;

         if(new_g < n->g)
         {
             n->g = new_g;

             n->h = heuristic(*n, *goal);
             n->f = n->g + n->h;

             n->parent_index = {node->x, node->y};

             m_opened.push(n);
         }
     }

}

// optional<vector<Node*>> a_star()
// {
//
//     m_opened.push(start);
//
//     start->g = 0 ;
//     start->h = heuristic(*start, *goal);
//     start->f = start->g + start->h;
//
//     while(!m_opened.empty())
//     {
//         Node* node = m_opened.top();
//         m_opened.pop();
//
//         if(node->is_obs || node->is_closed)
//             continue;
//
//         if(node == goal)
//         {
//             vector<Node*> path;
//
//             pair<int,int> parent_pos = goal->parent_index;
//             Node* current = goal; 
//             while (current && !current->is_start){
//                 current = &m_grid[parent_pos.second][parent_pos.first]; 
//                 parent_pos = current->parent_index;
//                 path.push_back(current);
//             }
//
//             return path;
//         }
//
//         node->is_closed = true;
//
//         for(Node* n : get_neighbors(node))
//         {
//             if(n->is_closed || n->is_obs)
//                 continue;
//
//             int new_g = node->g + 1;
//
//             if(new_g < n->g)
//             {
//                 n->g = new_g;
//
//                 n->h = heuristic(*n, *goal);
//                 n->f = n->g + n->h;
//
//                 n->parent_index = {node->x, node->y};
//
//                 m_opened.push(n);
//             }
//         }
//     }
//
//     return nullopt;
// }
//  
//

int main(){
    start->is_start = true;
    goal->is_end = true;

    InitWindow(WIDTH , HEIGHT, "Astar");
    InitAudioDevice();

    SetTargetFPS(100000000);

    Music bgMusic = LoadMusicStream("../audios/toby fox - UNDERTALE Soundtrack - 04 Fallen Down.ogg");
    customFont = LoadFontEx("../fonts/DeterminationMonoWebRegular-Z5oq.ttf", 80, 0  , 0);

    PlayMusicStream(bgMusic);

    for (int row = 0; row < BOXES_COUNT; row++) {
        for (int col = 0; col < BOXES_COUNT; col++) {
            m_grid[row][col].x = col;
            m_grid[row][col].y = row;
        }
    }

    m_opened.push(start);

    start->g = 0 ;
    start->h = heuristic(*start, *goal);
    start->f = start->g + start->h;

    DrawOBS();

    // auto path = a_star();
    //
    // if(path){
    //     // cout << path->size() << endl;
    //     for(Node* n : *path){
    //         n->in_current_path = true;
    //     }
    // }
    // else{
    //     // handle not found
    // }
    //
    

    while(!WindowShouldClose()) {
        UpdateMusicStream(bgMusic);


        BeginDrawing();
        
        ClearBackground(BLACK);

        DrawGrid();
        a_star_step();



        EndDrawing();
    }


    UnloadFont(customFont);
    UnloadMusicStream(bgMusic);
    CloseAudioDevice();

    return 0;
}
