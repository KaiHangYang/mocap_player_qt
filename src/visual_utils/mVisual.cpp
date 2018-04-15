#include "../../include/mVisual.h"


namespace mVTools {
    static int color_data_size = 1025;
    static int color_data_pad = 5;
    
    static const unsigned char color_data[] = {
        158, 1, 66, 255, 158, 2, 66, 255, 159, 2, 66, 255, 159, 3, 66, 255, 160, 4, 66, 255, 160, 5, 66, 255, 161, 5, 66, 255, 161, 6, 66, 255, 161, 7, 67, 255, 162, 7, 67, 255, 162, 8, 67, 255, 163, 9, 67, 255, 163, 10, 67, 255, 164, 10, 67, 255, 164, 11, 67, 255, 164, 12, 67, 255, 165, 12, 67, 255, 165, 13, 68, 255, 166, 13, 68, 255, 166, 14, 68, 255, 167, 14, 68, 255, 167, 15, 68, 255, 168, 16, 68, 255, 168, 16, 68, 255, 168, 17, 68, 255, 169, 17, 68, 255, 169, 18, 69, 255, 170, 18, 69, 255, 170, 19, 69, 255, 171, 19, 69, 255, 171, 20, 69, 255, 171, 20, 69, 255, 172, 21, 69, 255, 172, 21, 69, 255, 173, 22, 69, 255, 173, 22, 70, 255, 174, 23, 70, 255, 174, 23, 70, 255, 174, 23, 70, 255, 175, 24, 70, 255, 175, 24, 70, 255, 176, 25, 70, 255, 176, 25, 70, 255, 177, 26, 70, 255, 177, 26, 70, 255, 177, 27, 71, 255, 178, 27, 71, 255, 178, 27, 71, 255, 179, 28, 71, 255, 179, 28, 71, 255, 180, 29, 71, 255, 191, 41, 74, 255, 192, 42, 74, 255, 192, 42, 74, 255, 192, 43, 74, 255, 193, 43, 74, 255, 193, 43, 74, 255, 194, 44, 74, 255, 194, 44, 74, 255, 195, 45, 75, 255, 195, 45, 75, 255, 195, 46, 75, 255, 196, 46, 75, 255, 196, 46, 75, 255, 197, 47, 75, 255, 197, 47, 75, 255, 198, 48, 75, 255, 198, 48, 75, 255, 198, 48, 75, 255, 199, 49, 76, 255, 199, 49, 76, 255, 200, 50, 76, 255, 200, 50, 76, 255, 200, 50, 76, 255, 201, 51, 76, 255, 201, 51, 76, 255, 202, 52, 76, 255, 202, 52, 76, 255, 203, 52, 76, 255, 203, 53, 76, 255, 203, 53, 77, 255, 204, 54, 77, 255, 204, 54, 77, 255, 205, 54, 77, 255, 205, 55, 77, 255, 206, 55, 77, 255, 206, 56, 77, 255, 206, 56, 77, 255, 207, 56, 77, 255, 207, 57, 77, 255, 208, 57, 77, 255, 208, 57, 78, 255, 209, 58, 78, 255, 209, 58, 78, 255, 209, 59, 78, 255, 210, 59, 78, 255, 210, 59, 78, 255, 211, 60, 78, 255, 211, 60, 78, 255, 211, 61, 78, 255, 212, 61, 78, 255, 212, 61, 78, 255, 213, 62, 78, 255, 213, 62, 78, 255, 213, 62, 78, 255, 214, 63, 78, 255, 214, 63, 78, 255, 214, 63, 78, 255, 214, 64, 78, 255, 215, 64, 78, 255, 215, 65, 78, 255, 215, 65, 78, 255, 216, 65, 78, 255, 216, 66, 78, 255, 216, 66, 77, 255, 216, 66, 77, 255, 217, 67, 77, 255, 217, 67, 77, 255, 217, 67, 77, 255, 217, 68, 77, 255, 218, 68, 77, 255, 218, 68, 77, 255, 218, 69, 77, 255, 219, 69, 77, 255, 219, 70, 77, 255, 219, 70, 76, 255, 219, 70, 76, 255, 220, 71, 76, 255, 220, 71, 76, 255, 220, 71, 76, 255, 220, 72, 76, 255, 221, 72, 76, 255, 221, 72, 76, 255, 221, 73, 76, 255, 222, 73, 76, 255, 222, 73, 76, 255, 222, 74, 75, 255, 222, 74, 75, 255, 223, 75, 75, 255, 223, 75, 75, 255, 223, 75, 75, 255, 223, 76, 75, 255, 224, 76, 75, 255, 224, 76, 75, 255, 224, 77, 75, 255, 224, 77, 75, 255, 225, 77, 75, 255, 225, 78, 74, 255, 225, 78, 74, 255, 225, 79, 74, 255, 226, 79, 74, 255, 226, 79, 74, 255, 226, 80, 74, 255, 233, 89, 72, 255, 233, 90, 71, 255, 233, 90, 71, 255, 233, 91, 71, 255, 233, 91, 71, 255, 234, 91, 71, 255, 234, 92, 71, 255, 234, 92, 71, 255, 234, 92, 71, 255, 235, 93, 71, 255, 235, 93, 71, 255, 235, 94, 70, 255, 235, 94, 70, 255, 236, 94, 70, 255, 236, 95, 70, 255, 236, 95, 70, 255, 236, 95, 70, 255, 236, 96, 70, 255, 237, 96, 70, 255, 237, 96, 70, 255, 237, 97, 70, 255, 237, 97, 69, 255, 238, 98, 69, 255, 238, 98, 69, 255, 238, 98, 69, 255, 238, 99, 69, 255, 238, 99, 69, 255, 239, 99, 69, 255, 239, 100, 69, 255, 239, 100, 69, 255, 239, 101, 69, 255, 240, 101, 68, 255, 240, 101, 68, 255, 240, 102, 68, 255, 240, 102, 68, 255, 240, 102, 68, 255, 241, 103, 68, 255, 241, 103, 68, 255, 241, 104, 68, 255, 241, 104, 68, 255, 241, 104, 68, 255, 242, 105, 67, 255, 242, 105, 67, 255, 242, 106, 67, 255, 242, 106, 67, 255, 242, 106, 67, 255, 243, 107, 67, 255, 243, 107, 67, 255, 243, 107, 67, 255, 243, 108, 67, 255, 243, 108, 67, 255, 244, 109, 67, 255, 244, 109, 67, 255, 244, 110, 67, 255, 244, 110, 67, 255, 244, 111, 67, 255, 244, 111, 67, 255, 244, 112, 67, 255, 244, 113, 67, 255, 244, 113, 67, 255, 244, 114, 68, 255, 244, 114, 68, 255, 245, 115, 68, 255, 245, 115, 68, 255, 245, 116, 68, 255, 245, 116, 68, 255, 245, 117, 68, 255, 245, 117, 69, 255, 245, 118, 69, 255, 245, 119, 69, 255, 245, 119, 69, 255, 245, 120, 69, 255, 245, 120, 69, 255, 245, 121, 70, 255, 246, 121, 70, 255, 246, 122, 70, 255, 246, 122, 70, 255, 246, 123, 70, 255, 246, 123, 70, 255, 246, 124, 71, 255, 246, 124, 71, 255, 246, 125, 71, 255, 246, 125, 71, 255, 246, 126, 71, 255, 246, 126, 71, 255, 246, 127, 72, 255, 246, 128, 72, 255, 247, 128, 72, 255, 247, 129, 72, 255, 247, 129, 72, 255, 247, 130, 73, 255, 247, 130, 73, 255, 247, 131, 73, 255, 247, 131, 73, 255, 247, 132, 74, 255, 247, 132, 74, 255, 247, 133, 74, 255, 247, 133, 74, 255, 247, 134, 74, 255, 247, 134, 75, 255, 247, 135, 75, 255, 248, 135, 75, 255, 248, 136, 75, 255, 249, 149, 82, 255, 250, 150, 82, 255, 250, 150, 82, 255, 250, 151, 82, 255, 250, 151, 83, 255, 250, 152, 83, 255, 250, 152, 83, 255, 250, 153, 83, 255, 250, 153, 84, 255, 250, 154, 84, 255, 250, 154, 84, 255, 250, 155, 84, 255, 250, 155, 85, 255, 250, 156, 85, 255, 250, 156, 85, 255, 250, 157, 85, 255, 251, 157, 86, 255, 251, 158, 86, 255, 251, 158, 86, 255, 251, 159, 87, 255, 251, 159, 87, 255, 251, 160, 87, 255, 251, 160, 87, 255, 251, 161, 88, 255, 251, 161, 88, 255, 251, 162, 88, 255, 251, 162, 89, 255, 251, 162, 89, 255, 251, 163, 89, 255, 251, 163, 89, 255, 251, 164, 90, 255, 251, 164, 90, 255, 251, 165, 90, 255, 252, 165, 91, 255, 252, 166, 91, 255, 252, 166, 91, 255, 252, 167, 92, 255, 252, 167, 92, 255, 252, 168, 92, 255, 252, 168, 93, 255, 252, 169, 93, 255, 252, 169, 93, 255, 252, 170, 94, 255, 252, 170, 94, 255, 252, 171, 94, 255, 252, 171, 95, 255, 252, 172, 95, 255, 252, 172, 95, 255, 252, 172, 96, 255, 252, 173, 96, 255, 252, 173, 96, 255, 253, 174, 97, 255, 253, 174, 97, 255, 253, 175, 97, 255, 253, 175, 98, 255, 253, 175, 98, 255, 253, 176, 98, 255, 253, 176, 98, 255, 253, 177, 99, 255, 253, 177, 99, 255, 253, 178, 99, 255, 253, 178, 100, 255, 253, 178, 100, 255, 253, 179, 100, 255, 253, 179, 100, 255, 253, 180, 101, 255, 253, 180, 101, 255, 253, 180, 101, 255, 253, 181, 102, 255, 253, 181, 102, 255, 253, 182, 102, 255, 253, 182, 103, 255, 253, 182, 103, 255, 253, 183, 103, 255, 253, 183, 104, 255, 253, 184, 104, 255, 253, 184, 104, 255, 253, 184, 105, 255, 253, 185, 105, 255, 253, 185, 105, 255, 253, 186, 106, 255, 253, 186, 106, 255, 253, 186, 106, 255, 253, 187, 107, 255, 253, 187, 107, 255, 253, 188, 107, 255, 253, 188, 108, 255, 253, 188, 108, 255, 253, 189, 108, 255, 253, 189, 109, 255, 253, 190, 109, 255, 253, 190, 109, 255, 253, 190, 110, 255, 253, 191, 110, 255, 253, 191, 110, 255, 253, 192, 111, 255, 253, 192, 111, 255, 253, 192, 112, 255, 253, 193, 112, 255, 253, 193, 112, 255, 253, 194, 113, 255, 253, 194, 113, 255, 253, 204, 123, 255, 253, 205, 123, 255, 253, 205, 123, 255, 253, 205, 124, 255, 253, 206, 124, 255, 253, 206, 125, 255, 253, 207, 125, 255, 253, 207, 125, 255, 253, 207, 126, 255, 253, 208, 126, 255, 253, 208, 127, 255, 253, 209, 127, 255, 253, 209, 127, 255, 253, 209, 128, 255, 253, 210, 128, 255, 253, 210, 129, 255, 253, 211, 129, 255, 253, 211, 129, 255, 253, 211, 130, 255, 253, 212, 130, 255, 253, 212, 131, 255, 253, 212, 131, 255, 253, 213, 131, 255, 253, 213, 132, 255, 253, 214, 132, 255, 253, 214, 133, 255, 253, 214, 133, 255, 253, 215, 133, 255, 253, 215, 134, 255, 253, 215, 134, 255, 253, 216, 135, 255, 253, 216, 135, 255, 253, 217, 135, 255, 253, 217, 136, 255, 253, 217, 136, 255, 253, 218, 137, 255, 253, 218, 137, 255, 253, 218, 138, 255, 253, 219, 138, 255, 253, 219, 138, 255, 253, 220, 139, 255, 253, 220, 139, 255, 253, 220, 140, 255, 253, 221, 140, 255, 253, 221, 141, 255, 253, 221, 141, 255, 253, 222, 141, 255, 253, 222, 142, 255, 253, 223, 142, 255, 253, 223, 143, 255, 253, 223, 143, 255, 253, 224, 144, 255, 253, 224, 144, 255, 253, 224, 144, 255, 253, 224, 144, 255, 253, 224, 144, 255, 253, 224, 144, 255, 253, 225, 144, 255, 253, 225, 145, 255, 253, 225, 145, 255, 253, 225, 145, 255, 252, 225, 145, 255, 252, 225, 145, 255, 252, 225, 145, 255, 252, 226, 146, 255, 252, 226, 146, 255, 252, 226, 146, 255, 252, 226, 146, 255, 252, 226, 146, 255, 252, 226, 146, 255, 251, 227, 147, 255, 251, 227, 147, 255, 251, 227, 147, 255, 251, 227, 147, 255, 251, 227, 147, 255, 251, 227, 147, 255, 251, 227, 147, 255, 251, 228, 148, 255, 251, 228, 148, 255, 251, 228, 148, 255, 250, 228, 148, 255, 250, 228, 148, 255, 250, 228, 148, 255, 250, 229, 149, 255, 250, 229, 149, 255, 250, 229, 149, 255, 250, 229, 149, 255, 250, 229, 149, 255, 250, 229, 150, 255, 249, 229, 150, 255, 249, 230, 150, 255, 249, 230, 150, 255, 249, 230, 150, 255, 249, 230, 150, 255, 249, 230, 151, 255, 249, 230, 151, 255, 249, 230, 151, 255, 249, 231, 151, 255, 249, 231, 151, 255, 248, 231, 151, 255, 248, 231, 152, 255, 248, 231, 152, 255, 246, 235, 157, 255, 245, 235, 157, 255, 245, 235, 157, 255, 245, 236, 157, 255, 245, 236, 157, 255, 245, 236, 157, 255, 245, 236, 158, 255, 245, 236, 158, 255, 245, 236, 158, 255, 245, 236, 158, 255, 245, 237, 158, 255, 244, 237, 159, 255, 244, 237, 159, 255, 244, 237, 159, 255, 244, 237, 159, 255, 244, 237, 159, 255, 244, 238, 160, 255, 244, 238, 160, 255, 244, 238, 160, 255, 244, 238, 160, 255, 244, 238, 160, 255, 243, 238, 161, 255, 243, 238, 161, 255, 243, 239, 161, 255, 243, 239, 161, 255, 243, 239, 161, 255, 243, 239, 161, 255, 243, 239, 162, 255, 243, 239, 162, 255, 243, 239, 162, 255, 243, 240, 162, 255, 242, 240, 162, 255, 242, 240, 163, 255, 242, 240, 163, 255, 242, 240, 163, 255, 242, 240, 163, 255, 242, 240, 163, 255, 242, 241, 164, 255, 242, 241, 164, 255, 242, 241, 164, 255, 242, 241, 164, 255, 241, 241, 164, 255, 241, 241, 165, 255, 241, 241, 165, 255, 241, 242, 165, 255, 241, 242, 165, 255, 241, 242, 165, 255, 241, 242, 166, 255, 241, 242, 166, 255, 241, 242, 166, 255, 241, 242, 166, 255, 240, 243, 166, 255, 240, 243, 166, 255, 240, 243, 166, 255, 240, 243, 166, 255, 240, 243, 166, 255, 240, 243, 166, 255, 240, 243, 166, 255, 240, 243, 166, 255, 240, 243, 166, 255, 240, 243, 165, 255, 240, 243, 165, 255, 240, 243, 165, 255, 240, 243, 165, 255, 240, 243, 165, 255, 239, 243, 165, 255, 239, 243, 165, 255, 239, 243, 165, 255, 239, 243, 164, 255, 239, 243, 164, 255, 239, 243, 164, 255, 239, 243, 164, 255, 239, 243, 164, 255, 239, 243, 164, 255, 239, 243, 164, 255, 239, 243, 164, 255, 239, 243, 163, 255, 239, 243, 163, 255, 238, 243, 163, 255, 238, 243, 163, 255, 238, 243, 163, 255, 238, 243, 163, 255, 238, 243, 163, 255, 238, 243, 163, 255, 238, 243, 163, 255, 238, 243, 162, 255, 238, 243, 162, 255, 238, 243, 162, 255, 238, 243, 162, 255, 238, 243, 162, 255, 237, 243, 162, 255, 237, 243, 162, 255, 237, 243, 162, 255, 237, 243, 161, 255, 237, 243, 161, 255, 237, 243, 161, 255, 237, 243, 161, 255, 237, 243, 161, 255, 237, 243, 161, 255, 237, 243, 161, 255, 237, 243, 161, 255, 237, 243, 160, 255, 236, 243, 160, 255, 234, 244, 157, 255, 234, 244, 157, 255, 234, 244, 157, 255, 234, 244, 157, 255, 234, 244, 157, 255, 234, 244, 157, 255, 234, 244, 157, 255, 234, 244, 156, 255, 233, 244, 156, 255, 233, 244, 156, 255, 233, 244, 156, 255, 233, 244, 156, 255, 233, 244, 156, 255, 233, 244, 156, 255, 233, 244, 156, 255, 233, 244, 156, 255, 233, 244, 155, 255, 233, 244, 155, 255, 233, 244, 155, 255, 233, 244, 155, 255, 232, 244, 155, 255, 232, 244, 155, 255, 232, 244, 155, 255, 232, 244, 155, 255, 232, 244, 154, 255, 232, 244, 154, 255, 232, 244, 154, 255, 232, 244, 154, 255, 232, 244, 154, 255, 232, 244, 154, 255, 231, 244, 154, 255, 231, 244, 154, 255, 231, 244, 154, 255, 231, 244, 153, 255, 231, 244, 153, 255, 231, 244, 153, 255, 231, 244, 153, 255, 231, 244, 153, 255, 231, 244, 153, 255, 231, 244, 153, 255, 231, 244, 153, 255, 230, 244, 153, 255, 230, 244, 152, 255, 230, 244, 152, 255, 230, 244, 152, 255, 230, 244, 152, 255, 230, 244, 152, 255, 230, 244, 152, 255, 230, 244, 152, 255, 230, 244, 152, 255, 230, 244, 152, 255, 229, 244, 152, 255, 229, 244, 152, 255, 228, 244, 152, 255, 228, 244, 152, 255, 227, 244, 152, 255, 227, 244, 152, 255, 226, 243, 152, 255, 225, 243, 152, 255, 225, 243, 152, 255, 224, 243, 152, 255, 224, 243, 152, 255, 223, 243, 153, 255, 223, 242, 153, 255, 222, 242, 153, 255, 222, 242, 153, 255, 221, 242, 153, 255, 221, 242, 153, 255, 220, 242, 153, 255, 220, 241, 153, 255, 219, 241, 153, 255, 219, 241, 153, 255, 218, 241, 153, 255, 218, 241, 154, 255, 217, 241, 154, 255, 217, 240, 154, 255, 216, 240, 154, 255, 216, 240, 154, 255, 215, 240, 154, 255, 215, 240, 154, 255, 214, 240, 154, 255, 214, 239, 154, 255, 213, 239, 154, 255, 213, 239, 155, 255, 212, 239, 155, 255, 212, 239, 155, 255, 211, 238, 155, 255, 211, 238, 155, 255, 210, 238, 155, 255, 210, 238, 155, 255, 209, 238, 155, 255, 209, 238, 155, 255, 208, 237, 155, 255, 208, 237, 156, 255, 207, 237, 156, 255, 207, 237, 156, 255, 206, 237, 156, 255, 206, 236, 156, 255, 205, 236, 156, 255, 205, 236, 156, 255, 205, 236, 156, 255, 204, 236, 156, 255, 192, 231, 159, 255, 191, 231, 159, 255, 191, 230, 159, 255, 190, 230, 159, 255, 190, 230, 159, 255, 189, 230, 159, 255, 189, 230, 159, 255, 189, 229, 159, 255, 188, 229, 160, 255, 188, 229, 160, 255, 187, 229, 160, 255, 187, 229, 160, 255, 186, 228, 160, 255, 186, 228, 160, 255, 185, 228, 160, 255, 185, 228, 160, 255, 185, 228, 160, 255, 184, 227, 160, 255, 184, 227, 161, 255, 183, 227, 161, 255, 183, 227, 161, 255, 182, 227, 161, 255, 182, 226, 161, 255, 182, 226, 161, 255, 181, 226, 161, 255, 181, 226, 161, 255, 180, 226, 161, 255, 180, 225, 161, 255, 179, 225, 162, 255, 179, 225, 162, 255, 179, 225, 162, 255, 178, 225, 162, 255, 178, 224, 162, 255, 177, 224, 162, 255, 177, 224, 162, 255, 177, 224, 162, 255, 176, 224, 162, 255, 176, 223, 162, 255, 175, 223, 162, 255, 175, 223, 163, 255, 175, 223, 163, 255, 174, 223, 163, 255, 174, 222, 163, 255, 173, 222, 163, 255, 173, 222, 163, 255, 173, 222, 163, 255, 172, 222, 163, 255, 172, 221, 163, 255, 172, 221, 163, 255, 171, 221, 163, 255, 171, 221, 163, 255, 170, 220, 163, 255, 170, 220, 163, 255, 169, 220, 163, 255, 169, 220, 163, 255, 168, 220, 163, 255, 168, 219, 163, 255, 167, 219, 163, 255, 167, 219, 163, 255, 166, 219, 163, 255, 166, 219, 163, 255, 165, 218, 163, 255, 164, 218, 163, 255, 164, 218, 163, 255, 163, 218, 163, 255, 163, 218, 163, 255, 162, 217, 163, 255, 162, 217, 163, 255, 161, 217, 163, 255, 161, 217, 163, 255, 160, 217, 163, 255, 160, 216, 163, 255, 159, 216, 163, 255, 158, 216, 163, 255, 158, 216, 163, 255, 157, 216, 163, 255, 157, 215, 163, 255, 156, 215, 163, 255, 156, 215, 163, 255, 155, 215, 163, 255, 155, 215, 163, 255, 154, 214, 163, 255, 154, 214, 163, 255, 153, 214, 163, 255, 153, 214, 163, 255, 152, 214, 163, 255, 151, 213, 163, 255, 151, 213, 163, 255, 150, 213, 163, 255, 150, 213, 163, 255, 149, 213, 163, 255, 149, 212, 163, 255, 148, 212, 163, 255, 148, 212, 163, 255, 147, 212, 163, 255, 147, 212, 163, 255, 146, 211, 163, 255, 146, 211, 163, 255, 145, 211, 163, 255, 144, 211, 163, 255, 144, 210, 163, 255, 143, 210, 163, 255, 143, 210, 163, 255, 129, 204, 163, 255, 128, 204, 163, 255, 127, 204, 163, 255, 127, 204, 164, 255, 126, 204, 164, 255, 126, 203, 164, 255, 125, 203, 164, 255, 125, 203, 164, 255, 124, 203, 164, 255, 124, 203, 164, 255, 123, 202, 164, 255, 123, 202, 164, 255, 122, 202, 164, 255, 122, 202, 164, 255, 121, 201, 164, 255, 120, 201, 164, 255, 120, 201, 164, 255, 119, 201, 164, 255, 119, 201, 164, 255, 118, 200, 164, 255, 118, 200, 164, 255, 117, 200, 164, 255, 117, 200, 164, 255, 116, 200, 164, 255, 116, 199, 164, 255, 115, 199, 164, 255, 115, 199, 164, 255, 114, 199, 164, 255, 113, 198, 164, 255, 113, 198, 164, 255, 112, 198, 164, 255, 112, 198, 164, 255, 111, 198, 164, 255, 111, 197, 164, 255, 110, 197, 164, 255, 110, 197, 164, 255, 109, 197, 164, 255, 109, 196, 164, 255, 108, 196, 164, 255, 108, 196, 164, 255, 107, 196, 164, 255, 106, 196, 164, 255, 106, 195, 164, 255, 105, 195, 164, 255, 105, 195, 164, 255, 104, 195, 164, 255, 104, 194, 164, 255, 103, 194, 164, 255, 103, 194, 164, 255, 102, 194, 164, 255, 102, 194, 164, 255, 100, 193, 165, 255, 99, 193, 165, 255, 98, 193, 166, 255, 97, 192, 166, 255, 95, 192, 166, 255, 94, 191, 167, 255, 93, 191, 167, 255, 91, 191, 167, 255, 90, 190, 168, 255, 89, 190, 168, 255, 87, 190, 169, 255, 86, 189, 169, 255, 85, 189, 169, 255, 83, 189, 170, 255, 82, 188, 170, 255, 80, 188, 170, 255, 79, 187, 171, 255, 78, 187, 171, 255, 76, 187, 171, 255, 75, 186, 172, 255, 74, 186, 172, 255, 72, 186, 173, 255, 71, 185, 173, 255, 69, 185, 173, 255, 68, 184, 174, 255, 66, 184, 174, 255, 65, 184, 174, 255, 64, 183, 175, 255, 62, 183, 175, 255, 61, 182, 175, 255, 59, 182, 176, 255, 58, 182, 176, 255, 56, 181, 176, 255, 54, 181, 177, 255, 53, 180, 177, 255, 51, 180, 177, 255, 50, 179, 178, 255, 48, 179, 178, 255, 46, 179, 178, 255, 45, 178, 179, 255, 43, 178, 179, 255, 41, 177, 179, 255, 39, 177, 180, 255, 37, 176, 180, 255, 35, 176, 180, 255, 33, 176, 180, 255, 30, 175, 181, 255, 28, 175, 181, 255, 25, 174, 181, 255, 22, 174, 182, 255, 20, 173, 182, 255, 10, 161, 187, 255, 10, 161, 187, 255, 10, 160, 187, 255, 10, 160, 188, 255, 10, 159, 188, 255, 10, 159, 188, 255, 10, 158, 188, 255, 10, 158, 188, 255, 10, 158, 189, 255, 10, 157, 189, 255, 10, 157, 189, 255, 10, 156, 189, 255, 10, 156, 189, 255, 10, 155, 189, 255, 10, 155, 190, 255, 10, 154, 190, 255, 10, 154, 190, 255, 10, 153, 190, 255, 10, 153, 190, 255, 10, 152, 190, 255, 10, 152, 190, 255, 10, 151, 190, 255, 10, 151, 190, 255, 10, 150, 190, 255, 10, 150, 191, 255, 10, 149, 191, 255, 10, 149, 191, 255, 10, 148, 191, 255, 10, 148, 191, 255, 12, 147, 191, 255, 15, 147, 191, 255, 17, 146, 191, 255, 20, 146, 191, 255, 22, 145, 191, 255, 24, 145, 191, 255, 26, 144, 191, 255, 28, 143, 190, 255, 30, 143, 190, 255, 31, 142, 190, 255, 33, 142, 190, 255, 35, 141, 190, 255, 36, 141, 190, 255, 38, 140, 190, 255, 39, 140, 190, 255, 41, 139, 190, 255, 42, 139, 190, 255, 43, 138, 189, 255, 45, 138, 189, 255, 46, 137, 189, 255, 47, 136, 189, 255, 49, 136, 189, 255, 49, 135, 188, 255, 49, 135, 188, 255, 49, 135, 188, 255, 49, 134, 188, 255, 49, 134, 188, 255, 49, 133, 188, 255, 49, 133, 188, 255, 49, 133, 188, 255, 49, 132, 188, 255, 49, 132, 188, 255, 49, 132, 188, 255, 49, 131, 188, 255, 49, 131, 188, 255, 49, 130, 188, 255, 49, 130, 188, 255, 50, 130, 188, 255, 50, 129, 188, 255, 50, 129, 187, 255, 50, 128, 187, 255, 50, 128, 187, 255, 50, 128, 187, 255, 50, 127, 187, 255, 51, 127, 187, 255, 51, 126, 187, 255, 51, 126, 187, 255, 51, 126, 187, 255, 52, 125, 187, 255, 52, 125, 186, 255, 52, 124, 186, 255, 52, 124, 186, 255, 53, 124, 186, 255, 53, 123, 186, 255, 53, 123, 186, 255, 54, 122, 186, 255, 54, 122, 186, 255, 54, 121, 185, 255, 55, 121, 185, 255, 55, 121, 185, 255, 55, 120, 185, 255, 56, 120, 185, 255, 56, 119, 185, 255, 56, 119, 184, 255, 57, 118, 184, 255, 57, 118, 184, 255, 58, 118, 184, 255, 58, 117, 184, 255, 58, 117, 184, 255, 59, 116, 183, 255, 59, 116, 183, 255, 60, 115, 183, 255, 60, 115, 183, 255, 69, 103, 177, 255, 70, 103, 177, 255, 70, 102, 177, 255, 71, 102, 177, 255, 71, 101, 176, 255, 72, 101, 176, 255, 72, 101, 176, 255, 72, 100, 176, 255, 73, 100, 176, 255, 73, 99, 175, 255, 74, 99, 175, 255, 74, 98, 175, 255, 75, 98, 175, 255, 75, 97, 174, 255, 76, 97, 174, 255, 76, 96, 174, 255, 77, 96, 174, 255, 77, 95, 173, 254, 78, 95, 173, 246, 78, 95, 173, 239, 79, 94, 172, 231, 79, 94, 172, 224, 80, 93, 172, 216, 80, 93, 172, 209, 81, 92, 171, 201, 81, 92, 171, 194, 82, 91, 171, 186, 82, 91, 170, 179, 83, 90, 170, 171, 83, 90, 170, 164, 83, 89, 169, 156, 84, 89, 169, 149, 84, 88, 169, 141, 85, 88, 168, 134, 85, 87, 168, 127, 86, 87, 168, 119, 86, 86, 167, 112, 87, 86, 167, 104, 87, 85, 167, 97, 88, 85, 166, 89, 88, 84, 166, 82, 89, 84, 166, 74, 89, 83, 165, 67, 90, 83, 165, 59, 90, 82, 164, 52, 91, 82, 164, 44, 91, 81, 164, 37, 92, 81, 163, 29, 92, 80, 163, 22, 93, 80, 162, 14, 93, 79, 162, 7, 94, 79, 162, 0, 0, 0, 0, 0
    };

	int bone_nums = 14;
    int vnect_bone_nums = 16;
    std::vector<int>bone_indices({0,1, 1,2, 2,3, 3,4, 1,5, 5,6, 6,7, 1,14, 14,8, 8,9, 9,10, 14,11, 11,12, 12,13});
    std::vector<int>color_table({
        24, 0, 153,
        153, 7, 0,
        153, 102, 0,
        153, 153, 0,
        153, 51, 0,
        102, 153, 0,
        51, 153, 0,
        19, 152, 1,
        21, 153, 52,
        26, 153, 102,
        34, 153, 153,
        29, 102, 153,
        21, 51, 153,
        29, 102, 0
        });

    void drawLines(cv::Mat img, std::vector<glm::vec2> points) {
        int * b_i_ptr;
        int * c_ptr;
        int cur_bone_num;

        b_i_ptr = &bone_indices[0];
        c_ptr = &color_table[0];
        cur_bone_num = bone_nums;
        
        for (int i = 0; i < cur_bone_num; ++i) {
            glm::vec2 point_from = points[*(b_i_ptr++)];
            glm::vec2 point_to = points[*(b_i_ptr++)];

            cv::Point point_a(point_from.x, point_from.y);
            cv::Point point_b(point_to.x, point_to.y);

            cv::line(img, point_a, point_b, cv::Scalar(*(c_ptr+0), *(c_ptr+1), *(c_ptr+2)), 3);
            c_ptr += 3;
        }
    }

    void drawPoints(cv::Mat img, std::vector<glm::vec2> points) {
        int img_width, img_height;
        int p_num = points.size();
        int tmp_x, tmp_y;
        glm::vec2 * p_ptr = &points[0];

        img_width = img.size[0];
        img_height = img.size[1];

        for (int i = 0; i < p_num; ++i) {

            tmp_x = (*p_ptr).x;
            tmp_y = (*(p_ptr++)).y;
            cv::line(img, cv::Point(tmp_x-1>=0?tmp_x-1:0, tmp_y), cv::Point(tmp_x+1<img_width?tmp_x+1:img_width-1, tmp_y), cv::Scalar(0, 0, 255), 3);
            //cv::putText(img, std::to_string(i), cv::Point(tmp_x, tmp_y), cv::FONT_HERSHEY_PLAIN, 1.0, cv::Scalar(0, 255, 0));
        }
    }

    std::vector<unsigned char> getColor(float value, float max_val, float min_val, float mid_val) {
        // max_val -> red
        // min_val -> blue
        // min_val -> green
        
        value = std::max(value, min_val);
        value = std::min(max_val, value);
    
        float color_step = (color_data_size - 2*color_data_pad) / (max_val - min_val);
    
        float scale;
        float cur_min_val;
        int color_pos = std::floor(color_step * (value - min_val));
        cur_min_val = color_pos / color_step + min_val;
    
        color_pos = color_pos + color_data_pad;
        std::vector<float> min_color({(float)color_data[4*color_pos + 0], (float)color_data[4*color_pos + 1], (float)color_data[4*color_pos + 2], (float)color_data[4*color_pos + 3]});
        color_pos += 1;
        std::vector<float> max_color({(float)color_data[4*color_pos + 0], (float)color_data[4*color_pos + 1], (float)color_data[4*color_pos + 2], (float)color_data[4*color_pos + 3]});
        std::vector<unsigned char> result_color({0, 0, 0});
    
        scale = (value - cur_min_val) * color_step;
        result_color[0] = max_color[0] * (scale) + min_color[0] * (1 - scale);
        result_color[1] = max_color[1] * (scale) + min_color[1] * (1 - scale);
        result_color[2] = max_color[2] * (scale) + min_color[2] * (1 - scale);
        result_color[3] = max_color[3] * (scale) + min_color[3] * (1 - scale);
        return result_color;
    }

    void getRangeMap(cv::Mat & range_map, float max_val, float min_val, float mid_val, float type) {
        cv::Size r_map_size = range_map.size();
        if (r_map_size.width <= 0 || r_map_size.height <= 0) {
            type = 0;
            range_map = cv::Mat(50, 100, CV_8UC4);
            r_map_size = range_map.size();
        }

        if (range_map.channels() != 4) {
            range_map = cv::Mat(range_map.size(), CV_8UC4);
        }

        float step;
        if (type == 0) {
            step = (max_val - min_val) / r_map_size.width;
        }
        else {
            step = (max_val - min_val) / r_map_size.height;
        }

        unsigned char * r_ptr = range_map.ptr<unsigned char>(0);
        for (int i = 0; i < r_map_size.height; ++i) {
            for (int j = 0; j < r_map_size.width; ++j) {
                std::vector<unsigned char> color = getColor(j * step + min_val, max_val, min_val, mid_val);
                *(r_ptr++) = color[0];
                *(r_ptr++) = color[1];
                *(r_ptr++) = color[2];
                *(r_ptr++) = color[3];
            }
        }

    }
    
    void visualHeatmap(const cv::Mat & heatmap, cv::Mat &result, float max_val, float min_val, float mid_val) {
        cv::Size i_size = heatmap.size();
        result = cv::Mat(heatmap.size(), CV_8UC4, cv::Scalar(0));
    
        unsigned char * r_ptr = result.ptr<unsigned char>(0);
    
        const float * h_ptr = heatmap.ptr<float>(0);
    
        for (int i = 0; i < i_size.height; ++i) {
            for (int j = 0; j < i_size.width; ++j) {
                std::vector<unsigned char> color = getColor(*(h_ptr++), max_val, min_val, mid_val);
                *(r_ptr++) = color[0];
                *(r_ptr++) = color[1];
                *(r_ptr++) = color[2];
                *(r_ptr++) = color[3];
            }
        }
    }

    void visualPafmap(const cv::Mat & paf_x, const cv::Mat & paf_y, cv::Mat & result, float max_val, float min_val, float mid_val) {
        int paf_width = paf_x.size[1];
        int paf_height = paf_x.size[0];

        result = cv::Mat(paf_height, paf_width, CV_8UC4, cv::Scalar(0));

        unsigned char * r_ptr = result.ptr<unsigned char>(0);

        const float * x_ptr = paf_x.ptr<float>(0);
        const float * y_ptr = paf_y.ptr<float>(0);

        float angle;
        std::vector<unsigned char> pixel_color;

        for (int y = 0; y < paf_height; ++y) {
            for (int x = 0; x < paf_width; ++x) {
                glm::vec2 val(x_ptr[paf_width * y + x], y_ptr[paf_width * y + x]);
                // discard some noise
                if (std::abs(val.x) < 0.015 && std::abs(val.y) < 0.015) {
                    continue;
                }
                val = glm::normalize(val);
                angle = glm::acos(val.x);

                if (val.y < 0) {
                    angle = 2 * M_PI - angle;
                }

                angle = 180 * angle / M_PI;
                pixel_color = getColor(angle, max_val, min_val, mid_val);
                
                r_ptr[4*(paf_width * y + x) + 0] = pixel_color[0];
                r_ptr[4*(paf_width * y + x) + 1] = pixel_color[1];
                r_ptr[4*(paf_width * y + x) + 2] = pixel_color[2];
                r_ptr[4*(paf_width * y + x) + 3] = pixel_color[3];
            }
        }
    }

    void visualDzfmap(const cv::Mat & dzf, cv::Mat & result, float max_val, float min_val, float mid_val) {
        int dzf_width = dzf.size[1];
        int dzf_height = dzf.size[0];

        result = cv::Mat(dzf_height, dzf_width, CV_8UC4, cv::Scalar(0));
        unsigned char * r_ptr = result.ptr<unsigned char>(0);

        const float * d_ptr = dzf.ptr<float>(0);

        float val, angle;
        std::vector<unsigned char> pixel_color;
        
        for (int y = 0; y < dzf_height; ++y) {
            for (int x = 0; x < dzf_width; ++x) {
                val = d_ptr[dzf_width * y + x];
                angle = 180 * glm::asin(glm::abs(val)) / M_PI * (val>=0?1:-1);

                pixel_color = getColor(angle, max_val, min_val, mid_val);
                r_ptr[4*(dzf_width * y + x) + 0] = pixel_color[0];
                r_ptr[4*(dzf_width * y + x) + 1] = pixel_color[1];
                r_ptr[4*(dzf_width * y + x) + 2] = pixel_color[2];
                r_ptr[4*(dzf_width * y + x) + 3] = pixel_color[3];
            }
        }
    }
    // type 0 min val, type 1 max val
    void pixelWiseMinMax(const cv::Mat & mat1, cv::Mat & result_mat, int type) {
        cv::Size m_size = mat1.size();

        const float * m1_ptr = mat1.ptr<float>(0);
        float * r_ptr = result_mat.ptr<float>(0);

        for (int i = 0; i < m_size.height; ++i) {
            for (int j = 0; j < m_size.width; ++j) {
                if (type == 0) {
                    if (fabs(*r_ptr) <= 0.001){
                        *r_ptr = *m1_ptr;
                    }
                    else if (fabs(*m1_ptr) > 0.001 && *r_ptr > *m1_ptr) {
                        *r_ptr = *m1_ptr;
                    }
                }
                else if (type == 1) {
                    if (fabs(*r_ptr) <= 0.001) {
                        *r_ptr = *m1_ptr;
                    }
                    else if (fabs(*m1_ptr) > 0.001 && *r_ptr < *m1_ptr) {
                        *r_ptr = *m1_ptr;
                    }
                }
                else if (type == 2) {
                    if (fabs(*r_ptr) < fabs(*m1_ptr)) {
                        *r_ptr = *m1_ptr;
                    }
                }
                r_ptr ++;
                m1_ptr++;
            }
        }
    }
    void makeGaussian(cv::Mat & heatmap, float center_x, float center_y, float r) {
        cv::Size h_size = heatmap.size();
        float * h_ptr = heatmap.ptr<float>(0);
        for (int i = 0; i < h_size.height; ++i) {
            for (int j = 0; j < h_size.width; ++j) {
                *(h_ptr++) = exp(-((i-center_y) * (i-center_y) + (j-center_x) * (j-center_x) ) / 2.0 / r / r);
            }
        }
    }

};
