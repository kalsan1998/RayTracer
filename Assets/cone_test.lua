mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25)
mat4 = gr.material({0.7, 0.6, 1.0}, {0.5, 0.4, 0.8}, 25)

scene = gr.node( 'scene' )
scene:translate(0, 0, -800)


cone = gr.nh_cone('cone', {0, 0, 0}, 100)
cone:rotate('X', 70)
scene:add_child(cone)
cone:set_material(mat1)

l = gr.nh_sphere('s', {200, 0, 0}, 100)
scene:add_child(l)
l:set_material(mat1)

s = gr.nh_box('c', {-500, -1100, -100}, 1000)
scene:add_child(s)
s:set_material(mat2)

white_light = gr.light({-200.0, 300.0, 0.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, -650.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

gr.render(scene, './images/conetest.png', 512, 512,
	  {0, 0, 1}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})