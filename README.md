# Lazy Theta* with optimization pathfinding

Implementation of the algorithm described here: http://aigamedev.com/open/tutorial/lazy-theta-star/

At first I could find any code a part from this and honestly I had no idea of what line 37-38 means
![pseudo cat](http://aigamedev.com/wp-content/blogs.dir/5/files/2013/07/fig53-full.png)

After some amount of research I found this page http://idm-lab.org/project-o.html which provided me with some code.
I rewrote most of it because the code style dind't suite me and to take advantage of C++14.

I also made it more modular so that it can be used for grid, hexagonTile, polygon map, anything where you need to find a path between to point.

Also added the possibility to use weighted h-value as described in the first link, it make the search faster but may give slitghly longer path.
