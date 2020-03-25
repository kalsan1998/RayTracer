-- A simple scene with some miscellaneous geometry.

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0)
text1 = gr.texture('./textures/Bark_005_baseColor.png')
-- text2 = gr.texture('./textures/Wolf_Body.png')
-- text3 = gr.texture('./textures/stone_wall.png')
text4 = gr.texture('./textures/die.png')




scene_root = gr.node('root')

-- s1 = gr.nh_sphere('s1', {200, 0, -200}, 200)
-- scene_root:add_child(s1)
-- s1:add_texture(text3)

-- wolf = gr.mesh('wolf', './objects/Wolf_obj.obj')
-- scene_root:add_child(wolf)
-- wolf:add_texture(text2)
-- wolf:scale(100, 100, 100)
-- wolf:rotate('Y', -75);
-- wolf:translate(-70, 0, 500)

cylinder = gr.cone('cyl')
cylinder:scale(100, 300, 100)
cylinder:translate(0,200,200)
scene_root:add_child(cylinder)
cylinder:add_texture(text1)

-- cube = gr.nh_box('cube', {0, 0, 0}, 200)
-- scene_root:add_child(cube)
-- cube:add_texture(text4)
-- cube:rotate('Y', -45)

-- cube = gr.nh_box('cube', {0, 0, 0}, 200)
-- scene_root:add_child(cube)
-- cube:add_texture(text4)
-- cube:rotate('Y', -45)

white_light = gr.light({-100.0, 150.0, 80000.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene_root, './images/texture_test.png', 1024, 1024,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})