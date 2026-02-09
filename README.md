# RENDER IN C

*No filthy C++ business*


# **WHAT YOU NEED**
- PC
- **Linux with Wayland**
- make assets folder
- place objects in .obj format
- place textures in jpg/png

### Example:
```c
...
obj_parse_file(&data, "./assets/objects/basic_ship_textured.obj");
...
GLuint myTex = LoadTextureFromFile("./assets/textures/ship_texture.png");
glBindTexture(GL_TEXTURE_2D, myTex);
...
```

# **HOW TO RUN**
```$ make run```
