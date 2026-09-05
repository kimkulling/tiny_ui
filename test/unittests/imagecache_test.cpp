/*
MIT License

Copyright (c) 2022-2026 Kim Kulling

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "tinyui.h"

#include <gtest/gtest.h>

namespace tinyui {

class ImageCacheTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a test context for each test
        const Style style = TinyUi::getDefaultStyle();
        tui_log_func logger = [](LogSeverity severity, const char *message) {
            // stub
        };
        bool success = TinyUi::createContext("ImageCacheTest", style, logger);
        ASSERT_TRUE(success) << "Failed to create context for testing";
    }

    void TearDown() override {
        // Clean up the context after each test
        bool success = TinyUi::destroyContext();
        ASSERT_TRUE(success) << "Failed to destroy context after testing";
    }

    Context& getTestContext() {
        return TinyUi::getContext();
    }

    Image* createTestImage(int width = 100, int height = 100, int comp = 4) {
        auto* image = new Image();
        image->mSurfaceImpl = nullptr; // We don't need actual surface for testing
        image->mX = width;
        image->mY = height;
        image->mComp = comp;
        return image;
    }
};

// Test the ImageCache type alias
TEST_F(ImageCacheTest, ImageCacheTypeDefinition) {
    ImageCache cache;
    EXPECT_TRUE(cache.empty());
    EXPECT_EQ(cache.size(), 0);
}

// Test adding images to the cache
TEST_F(ImageCacheTest, AddImageToCache) {
    auto& ctx = getTestContext();
    Image* testImage = createTestImage();
    const char* imageName = "test_image.png";

    // Add image to cache
    ctx.addImage(imageName, testImage);

    // Verify image was added
    EXPECT_EQ(ctx.mImageCache.size(), 1);
    EXPECT_TRUE(ctx.mImageCache.find(imageName) != ctx.mImageCache.end());
    EXPECT_EQ(ctx.mImageCache[imageName], testImage);

    // Clean up
    delete testImage;
}

// Test adding null image or name
TEST_F(ImageCacheTest, AddNullImageOrName) {
    auto& ctx = getTestContext();
    Image* testImage = createTestImage();

    // Add with null name
    ctx.addImage(nullptr, testImage);
    EXPECT_EQ(ctx.mImageCache.size(), 0);

    // Add with null image
    ctx.addImage("null_image.png", nullptr);
    EXPECT_EQ(ctx.mImageCache.size(), 0);

    // Add with both null
    ctx.addImage(nullptr, nullptr);
    EXPECT_EQ(ctx.mImageCache.size(), 0);

    // Clean up
    delete testImage;
}

// Test getting images from cache
TEST_F(ImageCacheTest, GetImageFromCache) {
    auto& ctx = getTestContext();
    Image* testImage = createTestImage();
    const char* imageName = "get_test_image.png";

    // Add image to cache
    ctx.addImage(imageName, testImage);

    // Get image from cache
    Image* retrievedImage = ctx.getImage(imageName);
    EXPECT_EQ(retrievedImage, testImage);
    EXPECT_NE(retrievedImage, nullptr);

    // Get non-existent image
    Image* nullImage = ctx.getImage("non_existent.png");
    EXPECT_EQ(nullImage, nullptr);

    // Get with null name
    Image* nullResult = ctx.getImage(nullptr);
    EXPECT_EQ(nullResult, nullptr);

    // Clean up
    delete testImage;
}

// Test removing images from cache
TEST_F(ImageCacheTest, RemoveImageFromCache) {
    auto& ctx = getTestContext();
    Image* testImage = createTestImage();
    const char* imageName = "remove_test_image.png";

    // Add image to cache
    ctx.addImage(imageName, testImage);
    EXPECT_EQ(ctx.mImageCache.size(), 1);

    // Remove image from cache
    bool removed = ctx.removeImage(imageName);
    EXPECT_TRUE(removed);
    EXPECT_EQ(ctx.mImageCache.size(), 0);
    EXPECT_TRUE(ctx.mImageCache.find(imageName) == ctx.mImageCache.end());

    // Try to remove non-existent image
    bool notRemoved = ctx.removeImage("non_existent.png");
    EXPECT_FALSE(notRemoved);

    // Try to remove with null name
    bool nullRemoved = ctx.removeImage(nullptr);
    EXPECT_FALSE(nullRemoved);

    // Clean up
    delete testImage;
}

// Test multiple images in cache
TEST_F(ImageCacheTest, MultipleImagesInCache) {
    auto& ctx = getTestContext();

    // Add multiple images
    Image* image1 = createTestImage(100, 100);
    Image* image2 = createTestImage(200, 150);
    Image* image3 = createTestImage(50, 50);

    ctx.addImage("image1.png", image1);
    ctx.addImage("image2.png", image2);
    ctx.addImage("image3.png", image3);

    EXPECT_EQ(ctx.mImageCache.size(), 3);

    // Verify all images are retrievable
    EXPECT_EQ(ctx.getImage("image1.png"), image1);
    EXPECT_EQ(ctx.getImage("image2.png"), image2);
    EXPECT_EQ(ctx.getImage("image3.png"), image3);

    // Remove one image
    ctx.removeImage("image2.png");
    EXPECT_EQ(ctx.mImageCache.size(), 2);
    EXPECT_EQ(ctx.getImage("image2.png"), nullptr);
    EXPECT_EQ(ctx.getImage("image1.png"), image1);
    EXPECT_EQ(ctx.getImage("image3.png"), image3);

    // Clean up
    delete image1;
    delete image2;
    delete image3;
}

// Test duplicate image names (should overwrite)
TEST_F(ImageCacheTest, DuplicateImageNames) {
    auto& ctx = getTestContext();

    Image* firstImage = createTestImage(100, 100);
    Image* secondImage = createTestImage(200, 200);

    const char* imageName = "duplicate.png";

    // Add first image
    ctx.addImage(imageName, firstImage);
    EXPECT_EQ(ctx.getImage(imageName), firstImage);

    // Add second image with same name (should replace first)
    ctx.addImage(imageName, secondImage);
    EXPECT_EQ(ctx.getImage(imageName), secondImage);
    EXPECT_EQ(ctx.mImageCache.size(), 1);

    // Clean up
    delete firstImage;
    delete secondImage;
}

// Test image properties in cache
TEST_F(ImageCacheTest, ImagePropertiesInCache) {
    auto& ctx = getTestContext();

    const int testWidth = 128;
    const int testHeight = 64;
    const int testComp = 3;

    Image* testImage = createTestImage(testWidth, testHeight, testComp);
    ctx.addImage("props_test.png", testImage);

    Image* retrieved = ctx.getImage("props_test.png");
    ASSERT_NE(retrieved, nullptr);

    EXPECT_EQ(retrieved->mX, testWidth);
    EXPECT_EQ(retrieved->mY, testHeight);
    EXPECT_EQ(retrieved->mComp, testComp);

    // Clean up
    delete testImage;
}

// Test clearing cache manually (simulate releaseImageCache behavior)
TEST_F(ImageCacheTest, ManualCacheClearing) {
    auto& ctx = getTestContext();

    // Add some images
    Image* image1 = createTestImage();
    Image* image2 = createTestImage();

    ctx.addImage("clear1.png", image1);
    ctx.addImage("clear2.png", image2);
    EXPECT_EQ(ctx.mImageCache.size(), 2);

    // Manually clear the cache (simulating releaseImageCache)
    for (auto it = ctx.mImageCache.begin(); it != ctx.mImageCache.end(); ++it) {
        delete it->second; // Delete the image
    }
    ctx.mImageCache.clear();

    EXPECT_EQ(ctx.mImageCache.size(), 0);
    EXPECT_TRUE(ctx.mImageCache.empty());
}

// Test direct access to ImageCache
TEST_F(ImageCacheTest, DirectCacheAccess) {
    auto& ctx = getTestContext();

    Image* testImage = createTestImage();
    const char* imageName = "direct_test.png";
    ctx.addImage(imageName, testImage);

    // Test direct access to the underlying cache
    auto it = ctx.mImageCache.find(imageName);
    EXPECT_NE(it, ctx.mImageCache.end());
    EXPECT_EQ(it->second, testImage);

    // Test finding non-existent image
    auto notFoundIt = ctx.mImageCache.find("non_existent.png");
    EXPECT_EQ(notFoundIt, ctx.mImageCache.end());

    // Clean up
    delete testImage;
}

} // namespace tinyui
