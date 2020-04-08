scene = gr.node('scene')
grey = gr.material({0.5, 0.5, 0.5}, {0.0, 0.0, 0.0}, 0)
green = gr.material({0.08, 0.51, 0.03}, {0.0, 0.0, 0.0}, 0)
red = gr.material({0.83, 0.07, 0.07}, {0.0, 0.0, 0.0}, 0)

y_mirror = gr.material_refl({0.7,0.7,0.3}, {0.8, 0.8, 0.8}, 25, 0.3)
glass = gr.material_refr({0.6,0.6,0.6}, {0.9,0.9,0.9}, 25, 0.9, 1.5)

-- the floor
floor = gr.mesh( 'floor', './objects/plane.obj' )
scene:add_child(floor)
floor:scale(10, 10, 10)
floor:set_material(grey)

-- the wall
wall = gr.mesh( 'wall', './objects/plane.obj' )
scene:add_child(wall)
wall:scale(10, 10, 10)
wall:rotate('X', -90)
wall:translate(0,5,10)
wall:set_material(grey)

wall_l = gr.mesh( 'wall_l', './objects/plane.obj' )
scene:add_child(wall_l)
wall_l:scale(10, 10, 10)
wall_l:rotate('X', -90)
wall_l:rotate('Y', 90)
wall_l:translate(6,0,0)
wall_l:set_material(red)

wall_r = gr.mesh( 'wall_r', './objects/plane.obj' )
scene:add_child(wall_r)
wall_r:scale(10, 10, 10)
wall_r:rotate('X', -90)
wall_r:rotate('Y', -90)
wall_r:translate(-6, 0,0)
wall_r:set_material(green)

wall_b = gr.mesh( 'wall_b', './objects/plane.obj' )
scene:add_child(wall_b)
wall_b:scale(10, 10, 10)
wall_b:rotate('X', 90)
wall_b:translate(0,5,-20)
wall_b:set_material(grey)

wall_t = gr.mesh( 'wall_t', './objects/plane.obj' )
scene:add_child(wall_t)
wall_t:scale(10, 10, 10)
wall_t:rotate('X', 180)
wall_t:translate(0,10,0)
wall_t:set_material(grey)

sp1 = gr.sphere('sp1')
scene:add_child(sp1)
sp1:scale(1.5,1.5,1.5)
sp1:set_material(y_mirror)
sp1:translate(3, 1.5, 5)

sp2 = gr.sphere('sp2')
scene:add_child(sp2)
sp2:scale(2,2,2)
sp2:set_material(glass)
sp2:translate(0, 2, 0)

l1 = gr.light({0,9.9,0}, {0.9, 0.9, 0.9}, {1, 0.05, 0})

gr.render(scene, './images/room.png', 512, 512, 
	  {0, 5, -15}, {0, 0, 1}, {0, 1, 0}, 50,
	  {0.2, 0.2, 0.2}, {l1})