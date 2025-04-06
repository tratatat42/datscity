import fire
import json
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import numpy as np
from itertools import combinations, product

COLORS = [
    [1, 0, 0],
    [0, 1, 0],
    [0, 0, 1]
]
DIRS = {
    1: [0, 0, 1],
    2: [1, 0, 0],
    3: [0, 1, 0]
}


"""
# Function to plot a cube
def plot_cube(ax, position, color):
    r = [-0.5, 0.5]
    for s, e in combinations(np.array(list(product(r, r, r))), 2):
        if np.sum(np.abs(s-e)) == r[1]-r[0]:  # Only connect adjacent points
            ax.plot3D(*zip(s+position, e+position), color=color)
"""

def plot_cube(ax, position, color, alpha=0.5):
    # Define vertices for the cube
    vertices = np.array([
        [position[0] - 0.5, position[1] - 0.5, position[2] - 0.5],
        [position[0] + 0.5, position[1] - 0.5, position[2] - 0.5],
        [position[0] + 0.5, position[1] + 0.5, position[2] - 0.5],
        [position[0] - 0.5, position[1] + 0.5, position[2] - 0.5],
        [position[0] - 0.5, position[1] - 0.5, position[2] + 0.5],
        [position[0] + 0.5, position[1] - 0.5, position[2] + 0.5],
        [position[0] + 0.5, position[1] + 0.5, position[2] + 0.5],
        [position[0] - 0.5, position[1] + 0.5, position[2] + 0.5]
    ])

    # Define faces of the cube
    faces = [
        [vertices[0], vertices[1], vertices[2], vertices[3]], # Bottom face
        [vertices[4], vertices[5], vertices[6], vertices[7]], # Top face
        [vertices[0], vertices[1], vertices[5], vertices[4]], # Front face
        [vertices[2], vertices[3], vertices[7], vertices[6]], # Back face
        [vertices[1], vertices[2], vertices[6], vertices[5]], # Right face
        [vertices[4], vertices[7], vertices[3], vertices[0]]  # Left face
    ]

    # Create a Poly3DCollection for the cube and set transparency
    poly3d = Poly3DCollection(faces, alpha=alpha)
    poly3d.set_facecolor(color)  # Set face color
    ax.add_collection3d(poly3d)

def main(json_file='output.json'):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection='3d')

    with open(json_file, 'r') as f:
        data = json.load(f)
        ax.set_xlim([0, data['mapSize'][0]])
        ax.set_ylim([0, data['mapSize'][1]])
        ax.set_zlim([0, data['mapSize'][2]])

        cubes = [[[[0,0,0] for _ in range(data['mapSize'][2])] for _ in range(data['mapSize'][1])] for _ in range(data['mapSize'][0])]

        for idx, word in enumerate(data['words']):
            color = COLORS[idx % 3]
            position = word['pos']
            position[2] = -position[2]
            for i in range(word['len']):
                for j in range(3):
                    cubes[position[0]][position[1]][position[2]][j] = min(cubes[position[0]][position[1]][position[2]][j] + color[j], 1)
                for j in range(3):
                    position[j] += DIRS[word['dir']][j]

    for i in range(data['mapSize'][0]):
        for j in range(data['mapSize'][1]):
            for k in range(data['mapSize'][2]):
                if cubes[i][j][k][0] > 0 or cubes[i][j][k][1] > 0 or cubes[i][j][k][2] > 0:
                    plot_cube(ax, np.array([i,j,k]), tuple(cubes[i][j][k]))

    plt.show()


if __name__ == '__main__':
    fire.Fire(main)
