Mode 1: glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

This is probably the most typical blending mode. Source gets blended on top of destination proportional to its alpha value, giving a nice transparency effect. Nothing special here.

Mode 2: glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

I picked this at random and it looked cool, so I kept it. Color is used as transparency, so the darker colors in the image end up transparent, while lighter colors end up mostly opaque. This ended up giving my picture a pretty neat outline/sharpening effect, where the black lines in He-Man would blend through to the background, and the dark background in the original image almost disappears.