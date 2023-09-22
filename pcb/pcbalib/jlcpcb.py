#!/usr/bin/env python3
import os
import sys
import csv
from gerberex.utility import rotate

TOP='Top'
BOTTOM='Bottom'

def normalize_angle(angle):
    while angle >= 360:
        angle -= 360
    while angle < 0:
        angle += 360
    return angle

class Component:
    def __init__(self, name, position, rotation, layer):
        self.name = name
        self.position = position
        self.position_offset = (0, 0)
        self.rotation = rotation
        self.angle_offset = 0
        self.layer = layer

    def offset(self, dx, dy):
        self.position = (self.position[0] + dx, self.position[1] + dy)

    def rotate(self, angle, center=(0,0)):
        self.rotation = normalize_angle(self.rotation + angle)
        self.position = rotate(self.position[0], self.position[1],
                               angle, center)

    def str(self):
        offset = rotate(self.position_offset[0], self.position_offset[1], 
                        self.rotation + self.angle_offset, (0, 0))
        return '{0},{1:.3f}mm,{2:.3f}mm,{3},{4}'.format(
            self.name,
            self.position[0] + offset[0], self.position[1] + offset[1],
            self.layer, normalize_angle(self.rotation + self.angle_offset))
        

class MountFile:
    def __init__(self):
        self.components = []

    def load(self, path, layer, exclude={}):
        self.components = []
        with open(path, 'r') as file:
            self.components = []
            for line in file:
                data = line.replace('\n', '').split()
                if not exclude or not data[0] in exclude:
                    c = Component(
                        data[0],
                        (float(data[1]), float(data[2])),
                        float(data[3]),
                        layer)
                    self.components.append(c)
    
    def append(self, components):
        for component in components:
            self.components.append(component)

    def offset(self, dx, dy):
        for c in self.components:
            c.offset(dx, dy)

    def rotate(self, angle, center=(0,0)):
        for c in self.components:
            c.rotate(angle, center)


class Composition:
    def __init__(self):
        self.restrictions = {}
        self.mountfiles = []

    def setBom(self, path, angles = []):
        with open(path, 'r') as file:
            bom = csv.reader(file)
            title = True
            for data in bom:
                if title:
                    title = False
                else:
                    names = data[1].split(',')
                    footprint = data[2]
                    angle = angles[footprint] if footprint in angles else 0
                    for name in names:
                        self.restrictions[name.replace(' ', '')] = angle

    def merge(self, file):
        self.mountfiles.append(file)

    def dump(self, path):
        with open(path, 'w') as out:
            out.write('Designator,Mid X,Mid Y,Layer,Rotation\n')
            for f in self.mountfiles:
                for c in f.components:
                    if not self.restrictions or c.name in self.restrictions:
                        if type(self.restrictions[c.name]) == tuple:
                            restriction = self.restrictions[c.name]
                            c.position_offset = restriction[0]
                            c.angle_offset = restriction[1]
                        else:
                            c.angle_offset = self.restrictions[c.name]
                        out.write('{}\n'.format(c.str()))
