layout = [
    ('canvas', 500 * 500),
    ('colormap', 256 * 3),
    ('palette', 256),
    ('texture', 4096 * 4096),
    ('buttons', 8 * 4),
    ('axis', 8 * 4 * 6),
    ('time', 4),
    ('frame', 4),
    ('cursorx', 4),
    ('cursory', 4),
    ('stat_fps', 4),
    ('texture_view', 4 * 4),
    ('cam_eye', 4 * 3),
    ('cam_center', 4 * 3),
    ('cam_up', 4 * 3),
    ('cam_fov', 4),
    ('cam_viewport', 4 * 4),
    ('model', 4 * 16),
]

string = ''
addr = 0
for i, l in enumerate(layout):
    constant = 'NUX_RAM_' + l[0].upper()
    string += constant + '=' + '0x{:x},'.format(addr) + '\n'
    addr += l[1]
string += 'NUX_RAM_USER=0x{:x}'.format(addr) 

print(string)
