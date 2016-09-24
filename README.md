zero robotics
TODO:
1. Function to find nearest item not picked up.
2. Orient sphere such that point away from origin towards
   drop off area (sometimes when drop off area is close to 
   the edges, sphere goes out of bounds trying to get the
   item in the drop off area)
3. Own routine in place setTargetPosition
4. What is the minimal size of the SPS triangle that needs
   to be traced for reliable drop off operation ? The larger
   the triangle the more time spent drawing it, and less
   time to pick up items.

Notes:
1. Noticed that setTargetPostion automatically avoids opposing
   sphere.
2. 9/30 qualification: Get non-zero score. Relatively trivial 
   since just dropping SPS gets 