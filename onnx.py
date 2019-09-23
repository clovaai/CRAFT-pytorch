import torch
from torch.autograd import Variable
import cv2
import imgproc
from craft import CRAFT
from collections import OrderedDict


def copyStateDict(state_dict):
    if list(state_dict.keys())[0].startswith("module"):
        start_idx = 1
    else:
        start_idx = 0
    new_state_dict = OrderedDict()
    for k, v in state_dict.items():
        name = ".".join(k.split(".")[start_idx:])
        new_state_dict[name] = v
    return new_state_dict






# load net
net = CRAFT()     # initialize
net = net.cuda()
#net = torch.nn.DataParallel(net)

net.load_state_dict(copyStateDict(torch.load('./craft_mlt_25k.pth')))
net.eval()

# load data
image = imgproc.loadImage('./images/rajabu.png')

# resize
img_resized, target_ratio, size_heatmap = imgproc.resize_aspect_ratio(image, 1280, interpolation=cv2.INTER_LINEAR, mag_ratio=1.5)
ratio_h = ratio_w = 1 / target_ratio

# preprocessing
x = imgproc.normalizeMeanVariance(img_resized)
x = torch.from_numpy(x).permute(2, 0, 1)    # [h, w, c] to [c, h, w]
x = Variable(x.unsqueeze(0))                # [c, h, w] to [b, c, h, w]
x = x.cuda()

from collections import OrderedDict

# trace export
torch.onnx.export(net,
                  x,
                  'onnx/craft.onnx',
                  export_params=True,
                  verbose=True)
