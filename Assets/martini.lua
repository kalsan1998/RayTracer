brown = gr.material({0.3, 0.1, 0.1}, {0.0, 0.0, 0.0}, 0)
water = gr.material_refr({0,0,0}, {0,0,0}, 0, 1.0, 1.3)

wallpaper_text = gr.texture('./textures/striped_wallpaper.png')

hardwood_text = gr.texture('./textures/hardwood.png')
hardwood_mat = gr.material_refl({0,0,0}, {0,0,0}, 0.5, 0.1)

mona = gr.texture('./textures/mona.png')

scene = gr.node('scene')
scene:rotate('X', -15)
-- the floor
table = gr.mesh( 'table', './objects/plane.obj' )
scene:add_child(table)
table:add_texture(hardwood_text)
table:set_material(hardwood_mat)
table:scale(10, 10, 10)

-- the wall
wall = gr.mesh( 'wall', './objects/plane.obj' )
scene:add_child(wall)
wall:add_texture(wallpaper_text)
wall:scale(15, 15, 15)
wall:rotate('X', -90)
wall:translate(0,5,15)

-- painting
p = gr.node('p')
painting = gr.mesh( 'painting', './objects/painting.obj' )
painting:add_texture(mona)
painting:scale(3, 1, 4.5)
painting:rotate('Y', 180)
painting:rotate('X', -90)
painting:translate(3,6,14.999)
scene:add_child(painting)

cup = gr.cylinder('cup')
scene:add_child(cup)
cup:set_material(water)
cup:scale(2,4,2)
cup:translate(-2, 1, 0)

straw = gr.cylinder('straw')
straw:scale(0.2,4,0.2)
straw:set_material(brown)
straw:rotate('Z', 25)
straw:translate(-2.75, 3, 1)
scene:add_child(straw)

scene:rotate('X', -10)


-- The lights
l1 = gr.light({100,100,-200}, {0.8, 0.8, 0.8}, {1, 0, 0})

gr.render(scene, './images/martini.png', 256, 256, 
	  {0, 2, -15,}, {0, 0, 1}, {0, 1, 0}, 50,
	  {0.2, 0.2, 0.2}, {l1})