-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0)
text1 = gr.texture('./textures/Bark_005_baseColor.png')

scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, -200}, 200)
scene_root:add_child(s1)
s1:set_texture(text1)
s1:rotate('X', 45)


white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, './images/texture_test.png', 512, 512,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})