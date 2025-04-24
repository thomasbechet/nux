layout = [
    ('screen', 320 * 240),
    ('zbuffer', 320 * 240 * 4),
    ('colormap', 256 * 3),
    ('palette', 256),
    ('buttons', 8 * 4),
    ('axis', 8 * 4 * 6),
    ('time', 4),
    ('frame', 4),
    ('cursorx', 4),
    ('cursory', 4),
    ('stat_fps', 4),
    ('cam_pos', 4 * 3),
    ('cam_rot', 4 * 9),
    ('cam_fov', 4),
]

string = ''
addr = 0
for i, l in enumerate(layout):
    constant = 'NUX_RAM_' + l[0].upper()
    string += constant + '=' + '0x{:x},'.format(addr) + '\n'
    addr += l[1]

print(string)
