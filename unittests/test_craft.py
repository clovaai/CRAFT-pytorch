import unittest
import craft
import cv2
import os


class TestVFE(unittest.TestCase):
    def test_craft(self):
        root_dir = os.path.dirname(os.path.abspath(__file__))
        img = cv2.imread(os.path.join(root_dir, '../figures/test_figure.png'))
        bboxes, _, _ = craft.detect_text(img)
        self.assertEqual(len(bboxes), 2)
        self.assertEqual(len(bboxes[0]), 4)
        self.assertEqual(len(bboxes[0][0]), 2)
        self.assertEqual(int(bboxes[0][0][0]), 82)

if __name__ == '__main__':
    unittest.main()