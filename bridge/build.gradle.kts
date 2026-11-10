plugins {
    id("maven-publish")
    alias(libs.plugins.android.library)
}

afterEvaluate {
    publishing {
        publications {
            register<MavenPublication>("debug") {
                groupId = "org.tpkarras"
                artifactId = "BasicOboe"
                version = "0.0.1"

                from(components["debug"])
            }
        }
    }
}

android {
    namespace = "org.tpkarras.basicoboe.bridge"
    compileSdk {
        version = release(37)
    }

    defaultConfig {
        minSdk = 29

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        externalNativeBuild {
            cmake {
                arguments("-DANDROID_STL=c++_shared")
                cppFlags += ""
            }
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }

    buildFeatures {
        prefab = true
    }
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }

    publishing {
        singleVariant("release")
        singleVariant("debug") {
            withSourcesJar()
        }
    }

}

dependencies {
    implementation("com.google.oboe:oboe:1.11.0")
    implementation(libs.appcompat)
    testImplementation(libs.junit)
    androidTestImplementation(libs.espresso.core)
    androidTestImplementation(libs.ext.junit)
}