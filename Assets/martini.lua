white = gr.material({0.8, 0.8, 0.8}, {0.0, 0.0, 0.0}, 0)
glass = gr.material_refr({0.6,0.6,0.6}, {0.9,0.9,0.9}, 100, 0.7, 1.05)
-- glass = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25)
wine = gr.material_refr({0.42,0.1,0.08}, {0,0,0}, 0, 0.4, 1.3)


wallpaper_text = gr.texture('./textures/striped_wallpaper.png')
ceiling_text = gr.texture('./textures/yellow_wallpaper.png')

hardwood_text = gr.texture('./textures/hardwood.png')
hardwood_mat = gr.material_refl({0.0,0.0,0.0}, {0.7,0.7,0.7}, 100, 0.05)

mona = gr.texture('./textures/mona.png')

scene = gr.node('scene')
scene:rotate('X', -15)
-- the floor
table = gr.mesh( 'table', './objects/plane.obj' )
scene:add_child(table)
table:add_texture(hardwood_text)
table:set_material(hardwood_mat)
table:scale(15, 15, 15)

-- the wall
wall = gr.mesh( 'wall', './objects/plane.obj' )
scene:add_child(wall)
wall:add_texture(wallpaper_text)
wall:scale(15, 15, 15)
wall:rotate('X', -90)
wall:translate(0,5,15)

wall_l = gr.mesh( 'wall_l', './objects/plane.obj' )
scene:add_child(wall_l)
wall_l:add_texture(wallpaper_text)
wall_l:scale(15, 15, 15)
wall_l:rotate('X', -90)
wall_l:rotate('Y', 90)
wall_l:translate(15,5,0)

wall_r = gr.mesh( 'wall_r', './objects/plane.obj' )
scene:add_child(wall_r)
wall_r:add_texture(wallpaper_text)
wall_r:scale(15, 15, 15)
wall_r:rotate('X', -90)
wall_r:rotate('Y', -90)
wall_r:translate(-15,5,0)

wall_b = gr.mesh( 'wall_b', './objects/plane.obj' )
scene:add_child(wall_b)
wall_b:add_texture(wallpaper_text)
wall_b:scale(15, 30, 15)
wall_b:rotate('X', 90)
wall_b:translate(0,5,-15)

-- wall_t = gr.mesh( 'wall_t', './objects/plane.obj' )
-- scene:add_child(wall_t)
-- wall_t:add_texture(ceiling_text)
-- wall_t:scale(15, 15, 15)
-- wall_t:rotate('X', 180)
-- wall_t:translate(0,15,0)

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
cup:set_material(glass)
cup:scale(2,3,2)
cup:translate(-2, 1.5, 0)

drink = gr.cylinder('wine')
cup:add_child(drink)
drink:scale(0.85, 0.8, 0.85)
drink:translate(0,-0.1,0)
drink:set_material(wine)

drink_t = gr.sphere('wine_t')
drink:add_child(drink_t)
drink_t:scale(0.5,0.001,0.5)
drink_t:set_material(wine)
drink_t:translate(0,0.5,0)


straw = gr.cylinder('straw')
straw:scale(0.2,4,0.2)
straw:set_material(white)
straw:rotate('Z', 25)
straw:translate(-2.6, 2.5, 0)
scene:add_child(straw)

scene:rotate('X', -5)


-- The lights
l1 = gr.light({0,1,-8}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(scene, './images/martini.png', 256, 256, 
	  {-1, 2, -14}, {0, 0, 1}, {0, 1, 0}, 50,
	  {0.2, 0.2, 0.2}, {l1})