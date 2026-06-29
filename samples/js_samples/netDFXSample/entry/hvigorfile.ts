import { legacyHapTasks ,OhosPluginId ,OhosHapContext} from '@ohos/hvigor-ohos-plugin';
import { hvigor, HvigorNode, HvigorPlugin} from '@ohos/hvigor';
import {spawn,exec,execSync} from 'child_process'
import * as fs from 'fs';
import * as path from 'path';
import { PythonShell } from 'python-shell';
function containsPng(filename: string): boolean {
    return filename.toLowerCase().includes('png') || filename.toLowerCase().includes('bmp') || filename.toLowerCase().includes('jpg');
}
function copyDirectorySync(src: string, dest: string) {
    if (!fs.existsSync(dest)) {
        fs.mkdirSync(dest, { recursive: true });
    }

    const entries = fs.readdirSync(src, { withFileTypes: true });

    for (const entry of entries) {
        const srcPath = path.join(src, entry.name);
        const destPath = path.join(dest, entry.name);

        if (entry.isDirectory()) {
            copyDirectorySync(srcPath, destPath);
        } else {
            if (containsPng(srcPath)) {
                fs.copyFileSync(srcPath, destPath);
            }
        }
    }
}

function createDirSync(dir: string): void {
    if (!fs.existsSync(dir)) {
        createDirSync(path.dirname(dir));
        fs.mkdirSync(dir);
    }
}
function delay(milliseconds: number) {
    return new Promise<void>(resolve => {
        setTimeout(() => {
            resolve();
        }, milliseconds);
    });
}
function deletePngBinFiles(folderPath: string): void {
    fs.readdirSync(folderPath).forEach((file) => {
        const filePath = path.join(folderPath, file);
        if (fs.lstatSync(filePath).isFile() && file.endsWith('.png.bin')) {
            console.log(filePath);
            fs.unlinkSync(filePath);
        } else if (fs.lstatSync(filePath).isDirectory()) {
            deletePngBinFiles(filePath);
        }
    });
}
async function executeCommand(dirPath: string): Promise<void> {
    return new Promise((resolve, reject) => {
        const myVar = process.env.ImageTool;
        let options = { cwd: myVar };
        console.log(myVar);
        const result = execSync('python ' + myVar + "\\images_tool.py -p file -c config.xml -i "+dirPath+" -o "+dirPath, options);
        console.log(result.toString());
    });
}


export function customPlugin(): HvigorPlugin {
    return {
        pluginId: 'customPlugin',
        context() {
            return {
                data: 'customPlugin xxx'
            };
        },
        async apply(currentNode: HvigorNode): Promise<void> {
            currentNode.registerTask({
                // 任务名称
                name: `default@makepicbin`,
                // 任务执行逻辑主体函数
                async run() {
                    const hapContext = currentNode.getContext(OhosPluginId.OHOS_HAP_PLUGIN) as OhosHapContext;
                    hapContext?.targets((target: default) => {
                        const outputPath = target.getBuildTargetOutputPath();
                        console.log(outputPath);
                        const srcDir = `${target.projectPath}/entry/src/main/js/MainAbility`;
                        const destDir = `${target.projectPath}/entry/build/default/intermediates/lite_source/default/assets/js/MainAbility`;
                        const destDir2 = `${target.projectPath}/entry/build/default/intermediates/loader_out_lite/default/js/MainAbility`;
                        const testDir = `${target.projectPath}/test`;

                        try {
                            createDirSync(destDir);
                            console.log('目录创建成功');
                            createDirSync(destDir2);
                            console.log('目录创建成功');
                            copyDirectorySync(srcDir, destDir);
                            console.log('文件夹拷贝完成');
                            copyDirectorySync(srcDir, destDir2);
                            console.log('文件夹拷贝完成');
                            console.log(`${target.projectPath}/entry/build/default/intermediates`);
                            executeCommand(`${target.projectPath}/entry/build/default/intermediates`);
                            console.log('Command executed successfully11');
                            deletePngBinFiles(`${target.projectPath}/entry/build/default/intermediates`)
                        } catch (error) {
                            console.error('创建过程中出错:', error, destDir);
                        }
                    });
                },
                // 配置前置任务依赖
                dependencies: [
                    `default@LegacyCompileLiteJS`,
                    `default@LegacyCompileArkTS`,
                    `default@LegacyGenerateLoaderJson`,
                    `default@CacheNativeLibs`],
                // 配置任务的后置任务依赖
                postDependencies: [
                    `default@LegacyGenerateLiteCode`,
                    `default@LegacyBuildJS`,
                    `default@LegacyPackageHap`,
                    `default@SignHap`,
                    `assembleHap`
                ]
            });
        }
    };
}
export default {
    system: legacyHapTasks,  /* Built-in plugin of Hvigor. It cannot be modified. */
    plugins:[customPlugin()]               /* Custom plugin to extend the functionality of Hvigor. */
}
