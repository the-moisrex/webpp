import { spawn } from "node:child_process";
import { promises as fs } from "node:fs";
import path from "node:path";
import { fileURLToPath } from "node:url";

import { McpServer } from "@modelcontextprotocol/sdk/server/mcp.js";
import { StdioServerTransport } from "@modelcontextprotocol/sdk/server/stdio.js";
import { z } from "zod";

const thisFile = fileURLToPath(import.meta.url);
const defaultProjectRoot = path.resolve(path.dirname(thisFile), "../..");
const projectRoot = path.resolve(process.env.PROJECT_ROOT ?? defaultProjectRoot);
const maxReadableBytes = 512 * 1024;
const commandOutputLimit = 1024 * 1024;

const allowedCommands = new Set(["git", "cmake", "ctest", "rg"]);
const documentationGroups = {
  overview: ["README.md", "webpp/README.md", "docs/index.md"],
  http: ["webpp/http/README.md", "webpp/headers/README.md", "webpp/http/bodies/README.md"],
  uri: ["webpp/uri/README.md"],
  unicode: ["webpp/unicode/README.md", "webpp/unicode/details/README.md"],
  traits: ["webpp/traits/README.md", "webpp/std/README.md", "webpp/memory/README.md"],
  io: ["webpp/io/README.md"],
  storage: ["webpp/storage/README.md"],
  middleware: ["webpp/middleware/README.md"],
  sdk: ["sdk/README.md"],
  benchmarks: ["benchmarks/README.md"]
} as const;

const searchAreas = {
  repository: ".",
  library: "webpp",
  tests: "tests",
  docs: "docs",
  examples: "examples",
  benchmarks: "benchmarks",
  sdk: "sdk",
  cmake: "cmake"
} as const;

type CommandResult = {
  command: string;
  exitCode: number | null;
  stdout: string;
  stderr: string;
  truncated: boolean;
};

type NamedPreset = {
  name?: string;
  hidden?: boolean;
  binaryDir?: string;
  configurePreset?: string;
  inherits?: string | string[];
  targets?: string | string[];
};

type CMakePresets = {
  configurePresets?: NamedPreset[];
  buildPresets?: NamedPreset[];
  testPresets?: NamedPreset[];
};

function textResponse(text: string) {
  return {
    content: [{ type: "text" as const, text }]
  };
}

function isInsideRoot(resolvedPath: string): boolean {
  const relative = path.relative(projectRoot, resolvedPath);
  return relative === "" || (!relative.startsWith("..") && !path.isAbsolute(relative));
}

function relativeProjectPath(resolvedPath: string): string {
  return path.relative(projectRoot, resolvedPath).replaceAll(path.sep, "/");
}

function resolveInsideProject(inputPath: string): string {
  if (inputPath.trim() === "") {
    throw new Error("Path is required.");
  }
  if (inputPath.includes("\0")) {
    throw new Error("Path contains a null byte.");
  }

  const resolved = path.resolve(projectRoot, inputPath);
  if (!isInsideRoot(resolved)) {
    throw new Error("Path is outside the repository root.");
  }
  return resolved;
}

function isGeneratedTopLevel(name: string): boolean {
  return name === "build" || name.startsWith("build-") || name.startsWith("cmake-build-");
}

function rejectUnsafeReadPath(resolvedPath: string): void {
  const relative = relativeProjectPath(resolvedPath);
  const parts = relative.split("/").filter(Boolean);
  const basename = path.basename(relative).toLowerCase();
  const lowerRelative = relative.toLowerCase();

  if (parts.includes(".git") || lowerRelative.startsWith(".git/")) {
    throw new Error("Reading .git internals is not allowed.");
  }
  if (parts[0] && isGeneratedTopLevel(parts[0])) {
    throw new Error("Reading build artifacts is not allowed.");
  }
  if (parts.includes("node_modules")) {
    throw new Error("Reading node_modules is not allowed.");
  }
  if (parts[0] === "mcp" && parts[1] === "dist") {
    throw new Error("Reading mcp/dist is not allowed.");
  }
  if (basename === ".env" || basename.startsWith(".env.")) {
    throw new Error("Reading .env files is not allowed.");
  }
  if (
    basename.includes("secret") ||
    basename.includes("private") ||
    basename === "id_rsa" ||
    basename === "id_ed25519" ||
    basename.endsWith(".pem") ||
    basename.endsWith(".key") ||
    basename.endsWith(".p12") ||
    basename.endsWith(".pfx")
  ) {
    throw new Error("Reading secrets or private keys is not allowed.");
  }
}

async function readProjectFileText(inputPath: string): Promise<string> {
  const resolved = resolveInsideProject(inputPath);
  rejectUnsafeReadPath(resolved);

  const stat = await fs.stat(resolved);
  if (!stat.isFile()) {
    throw new Error("Path is not a regular file.");
  }
  if (stat.size > maxReadableBytes) {
    throw new Error(`File is larger than ${maxReadableBytes} bytes.`);
  }

  return fs.readFile(resolved, "utf8");
}

function formatCommand(command: string, args: string[]): string {
  return [command, ...args].map((part) => (/\s/.test(part) ? JSON.stringify(part) : part)).join(" ");
}

function formatCommandResult(result: CommandResult): string {
  const truncated = result.truncated ? "\n[output truncated]" : "";
  return [
    `$ ${result.command}`,
    `exit_code: ${result.exitCode ?? "unknown"}`,
    "",
    "stdout:",
    result.stdout || "(empty)",
    "",
    "stderr:",
    result.stderr || "(empty)",
    truncated
  ].join("\n");
}

async function runAllowedCommand(command: string, args: string[]): Promise<CommandResult> {
  if (!allowedCommands.has(command)) {
    throw new Error(`Command is not allowed: ${command}`);
  }

  return new Promise((resolve) => {
    const child = spawn(command, args, {
      cwd: projectRoot,
      env: process.env,
      shell: false
    });

    let stdout = "";
    let stderr = "";
    let truncated = false;

    const append = (current: string, chunk: Buffer): string => {
      if (current.length >= commandOutputLimit) {
        truncated = true;
        return current;
      }
      const next = current + chunk.toString("utf8");
      if (next.length > commandOutputLimit) {
        truncated = true;
        return next.slice(0, commandOutputLimit);
      }
      return next;
    };

    child.stdout.on("data", (chunk: Buffer) => {
      stdout = append(stdout, chunk);
    });
    child.stderr.on("data", (chunk: Buffer) => {
      stderr = append(stderr, chunk);
    });
    child.on("error", (error) => {
      stderr = append(stderr, Buffer.from(String(error)));
    });
    child.on("close", (exitCode) => {
      resolve({
        command: formatCommand(command, args),
        exitCode,
        stdout,
        stderr,
        truncated
      });
    });
  });
}

async function pathExists(inputPath: string): Promise<boolean> {
  try {
    await fs.access(inputPath);
    return true;
  } catch {
    return false;
  }
}

async function listExisting(paths: readonly string[]): Promise<string[]> {
  const existing: string[] = [];
  for (const item of paths) {
    if (await pathExists(path.join(projectRoot, item))) {
      existing.push(item);
    }
  }
  return existing;
}

async function shallowTree(directory: string, depth = 0, maxDepth = 2): Promise<string[]> {
  if (depth > maxDepth) {
    return [];
  }

  const resolved = path.join(projectRoot, directory);
  const entries = await fs.readdir(resolved, { withFileTypes: true });
  const visible = entries
    .filter((entry) => {
      const relative = `${directory}/${entry.name}`.replace(/^\.\//, "");
      const parts = relative.split("/");
      return (
        entry.name !== ".git" &&
        entry.name !== ".DS_Store" &&
        entry.name !== "node_modules" &&
        entry.name !== "dist" &&
        !(parts[0] && isGeneratedTopLevel(parts[0]))
      );
    })
    .sort((left, right) => {
      if (left.isDirectory() !== right.isDirectory()) {
        return left.isDirectory() ? -1 : 1;
      }
      return left.name.localeCompare(right.name);
    });

  const lines: string[] = [];
  for (const entry of visible) {
    const relative = path.posix.join(directory === "." ? "" : directory, entry.name);
    lines.push(`${"  ".repeat(depth)}${entry.isDirectory() ? `${relative}/` : relative}`);
    if (entry.isDirectory()) {
      lines.push(...(await shallowTree(relative, depth + 1, maxDepth)));
    }
  }
  return lines;
}

async function readCMakePresets(): Promise<CMakePresets> {
  const content = await readProjectFileText("CMakePresets.json");
  return JSON.parse(content) as CMakePresets;
}

function presetNames(presets: NamedPreset[] | undefined, includeHidden = false): string[] {
  return (
    presets
      ?.filter((preset) => includeHidden || !preset.hidden)
      .map((preset) => preset.name)
      .filter((name): name is string => Boolean(name)) ?? []
  );
}

function ensurePresetExists(presets: NamedPreset[] | undefined, name: string, kind: string): void {
  if (!presetNames(presets, true).includes(name)) {
    throw new Error(`${kind} preset does not exist: ${name}`);
  }
}

function expandPresetPath(inputPath: string): string {
  return inputPath
    .replaceAll("${sourceDir}", projectRoot)
    .replaceAll("${sourceParentDir}", path.dirname(projectRoot))
    .replaceAll("${sourceDirName}", path.basename(projectRoot));
}

function resolveSafeBuildDir(inputPath: string): string {
  const resolved = resolveInsideProject(inputPath);
  const relative = relativeProjectPath(resolved);
  const topLevel = relative.split("/")[0];
  if (!topLevel || !isGeneratedTopLevel(topLevel)) {
    throw new Error("Build directory must be a top-level build/ or build-* / cmake-build-* path.");
  }
  return resolved;
}

function inheritedPresetNames(preset: NamedPreset): string[] {
  if (!preset.inherits) {
    return [];
  }
  return Array.isArray(preset.inherits) ? preset.inherits : [preset.inherits];
}

function findConfigureBinaryDir(presets: CMakePresets, presetName: string, visited = new Set<string>()): string | undefined {
  if (visited.has(presetName)) {
    throw new Error(`Circular configure preset inheritance involving: ${presetName}`);
  }
  visited.add(presetName);

  const preset = presets.configurePresets?.find((candidate) => candidate.name === presetName);
  if (!preset) {
    return undefined;
  }
  if (preset.binaryDir) {
    return preset.binaryDir;
  }
  for (const inheritedName of inheritedPresetNames(preset)) {
    const inheritedBinaryDir = findConfigureBinaryDir(presets, inheritedName, visited);
    if (inheritedBinaryDir) {
      return inheritedBinaryDir;
    }
  }
  return undefined;
}

function resolveConfigureBuildDir(presets: CMakePresets, presetName: string): string {
  ensurePresetExists(presets.configurePresets, presetName, "Configure");
  const binaryDir = findConfigureBinaryDir(presets, presetName);
  if (!binaryDir) {
    throw new Error(`Configure preset has no resolvable binaryDir: ${presetName}`);
  }
  return resolveSafeBuildDir(expandPresetPath(binaryDir));
}

function escapeRegularExpression(value: string): string {
  return value.replace(/[.*+?^${}()|[\]\\]/g, "\\$&");
}

async function listTestTargets(): Promise<string[]> {
  const entries = await fs.readdir(path.join(projectRoot, "tests"), { withFileTypes: true });
  return entries
    .filter((entry) => entry.isFile() && entry.name.endsWith("_test.cpp"))
    .map((entry) => `test-${entry.name.slice(0, -"_test.cpp".length).replaceAll("_", "-")}`)
    .sort();
}

function formatPresetGroup(title: string, presets: NamedPreset[] | undefined): string[] {
  const visible = presets?.filter((preset) => !preset.hidden) ?? [];
  return [
    `${title}:`,
    ...visible.map((preset) => {
      const targets = Array.isArray(preset.targets) ? preset.targets.join(", ") : preset.targets;
      const details = [
        preset.configurePreset ? `configure=${preset.configurePreset}` : "",
        targets ? `targets=${targets}` : ""
      ].filter(Boolean);
      return `- ${preset.name ?? "(unnamed)"}${details.length > 0 ? ` (${details.join("; ")})` : ""}`;
    })
  ];
}

const server = new McpServer({
  name: "webpp-project",
  version: "1.0.0"
});

server.tool(
  "project_overview",
  "Describe the actual Web++ repository layout, authoritative files, major library subsystems, tests, and CMake entry points.",
  {},
  async () => {
    const importantFiles = await listExisting([
      "README.md",
      "webpp/README.md",
      "CMakeLists.txt",
      "CMakePresets.json",
      "webpp/CMakeLists.txt",
      "tests/CMakeLists.txt",
      ".clang-format",
      ".clang-tidy",
      "AGENTS.md",
      "CLAUDE.md",
      ".mcp.json"
    ]);
    const sourceFolders = await listExisting([
      "webpp/http",
      "webpp/uri",
      "webpp/unicode",
      "webpp/traits",
      "webpp/memory",
      "webpp/io",
      "webpp/storage",
      "webpp/protocol",
      "sdk"
    ]);
    const developmentFolders = await listExisting(["tests", "examples", "benchmarks", "cmake", "docs", "mcp"]);
    const tree = await shallowTree(".");

    return textResponse(
      [
        "project: Web++ cross-platform C++ web framework",
        `project_root: ${projectRoot}`,
        "library_layout: predominantly template/header implementation under webpp/, exposed as a CMake static target",
        "language_baseline: C++20 for the library; tests and GCC development targets may use C++23",
        "authority: implementation/tests > CMake/CI > component docs > old prose/examples",
        "",
        "important_files:",
        ...importantFiles.map((item) => `- ${item}`),
        "",
        "major_subsystems:",
        ...sourceFolders.map((item) => `- ${item}`),
        "",
        "development_folders:",
        ...developmentFolders.map((item) => `- ${item}`),
        "",
        "shallow_tree:",
        ...tree.map((item) => `- ${item}`)
      ].join("\n")
    );
  }
);

server.tool(
  "read_project_file",
  "Safely read a UTF-8 text file inside the Web++ repository, excluding build output, dependencies, Git internals, and secrets.",
  {
    path: z.string().min(1).describe("Repository-relative path to read.")
  },
  async ({ path: filePath }) => {
    const content = await readProjectFileText(filePath);
    return textResponse(`file: ${relativeProjectPath(resolveInsideProject(filePath))}\n\n${content}`);
  }
);

server.tool(
  "read_project_docs",
  "Read the checked-in Web++ documentation for one subsystem. Some prose is stale; compare it with implementation and tests.",
  {
    topic: z
      .enum([
        "overview",
        "http",
        "uri",
        "unicode",
        "traits",
        "io",
        "storage",
        "middleware",
        "sdk",
        "benchmarks"
      ])
      .default("overview")
      .describe("Documentation group to read.")
  },
  async ({ topic }) => {
    const files = documentationGroups[topic];
    const sections: string[] = [];
    for (const file of files) {
      if (await pathExists(path.join(projectRoot, file))) {
        sections.push(`## ${file}\n\n${await readProjectFileText(file)}`);
      } else {
        sections.push(`## ${file}\n\nMISSING`);
      }
    }
    return textResponse(sections.join("\n\n---\n\n"));
  }
);

server.tool(
  "search_project",
  "Search Web++ source, tests, docs, examples, SDK, benchmarks, or CMake files with ripgrep.",
  {
    query: z.string().min(1).max(500).describe("Literal or regular-expression pattern accepted by ripgrep."),
    area: z
      .enum(["repository", "library", "tests", "docs", "examples", "benchmarks", "sdk", "cmake"])
      .default("repository")
      .describe("Repository area to search."),
    glob: z.string().min(1).max(200).optional().describe("Optional ripgrep glob such as '*.hpp'.")
  },
  async ({ query, area, glob }) => {
    const args = [
      "-n",
      "--hidden",
      "--glob",
      "!.git/**",
      "--glob",
      "!mcp/node_modules/**",
      "--glob",
      "!build*/**",
      "--glob",
      "!cmake-build-*/**"
    ];
    if (glob) {
      args.push("--glob", glob);
    }
    args.push("--", query, searchAreas[area]);
    return textResponse(formatCommandResult(await runAllowedCommand("rg", args)));
  }
);

server.tool("git_status", "Run git status --short for the Web++ worktree.", {}, async () => {
  return textResponse(formatCommandResult(await runAllowedCommand("git", ["status", "--short"])));
});

server.tool(
  "git_diff",
  "Run git diff for tracked Web++ changes, or git diff --staged when staged is true.",
  {
    staged: z.boolean().default(false).describe("Return the staged diff instead of the working-tree diff.")
  },
  async ({ staged }) => {
    const args = staged ? ["diff", "--staged"] : ["diff"];
    return textResponse(formatCommandResult(await runAllowedCommand("git", args)));
  }
);

server.tool(
  "list_cmake_presets",
  "List usable Web++ configure, build, and test presets from CMakePresets.json.",
  {},
    async () => {
      const presets = await readCMakePresets();
      const testTargets = await listTestTargets();
      const buildPresetNames = new Set(presetNames(presets.buildPresets, true));
      const testPresetNames = new Set(
        presetNames(presets.buildPresets, true).filter((name) => name.startsWith("test-"))
      );
      const missingFocusedPresets = testTargets.filter((target) => !buildPresetNames.has(target));
      const staleFocusedPresets = [...testPresetNames].filter((target) => !testTargets.includes(target));
      return textResponse(
        [
          ...formatPresetGroup("configure_presets", presets.configurePresets),
        "",
          ...formatPresetGroup("build_presets", presets.buildPresets),
          "",
          ...formatPresetGroup("test_presets", presets.testPresets),
          "",
          "focused_test_preset_gaps:",
          ...(missingFocusedPresets.length > 0 ? missingFocusedPresets.map((name) => `- ${name}`) : ["- none"]),
          "",
          "stale_focused_test_presets:",
          ...(staleFocusedPresets.length > 0 ? staleFocusedPresets.map((name) => `- ${name}`) : ["- none"])
        ].join("\n")
      );
    }
);

server.tool(
  "list_test_targets",
  "List focused CMake/CTest target names derived from current tests/*_test.cpp files and show whether each has a build preset.",
  {},
  async () => {
    const presets = await readCMakePresets();
    const buildPresetNames = new Set(presetNames(presets.buildPresets, true));
    return textResponse(
      (await listTestTargets())
        .map((target) => `- ${target} (${buildPresetNames.has(target) ? "preset" : "direct CMake target"})`)
        .join("\n")
    );
  }
);

server.tool(
  "run_cmake_configure",
  "Run cmake --preset for a validated Web++ configure preset. CI development configuration is dev-default.",
  {
    preset: z.string().min(1).default("dev-default").describe("Configure preset from CMakePresets.json.")
  },
  async ({ preset }) => {
    const presets = await readCMakePresets();
    ensurePresetExists(presets.configurePresets, preset, "Configure");
    return textResponse(formatCommandResult(await runAllowedCommand("cmake", ["--preset", preset])));
  }
);

server.tool(
  "run_cmake_build",
  "Build a validated Web++ build preset, such as a focused test target, tests, examples, or benchmarks.",
  {
    preset: z.string().min(1).describe("Build preset from CMakePresets.json.")
  },
  async ({ preset }) => {
    const presets = await readCMakePresets();
    ensurePresetExists(presets.buildPresets, preset, "Build");
    return textResponse(formatCommandResult(await runAllowedCommand("cmake", ["--build", "--preset", preset])));
  }
);

server.tool(
  "run_ctest",
  "Run a validated Web++ CTest preset, optionally filtering to one or more test names.",
  {
    preset: z.string().min(1).default("tests").describe("CTest preset from CMakePresets.json."),
    filter: z.string().min(1).max(500).optional().describe("Optional CTest -R regular expression.")
  },
  async ({ preset, filter }) => {
    const presets = await readCMakePresets();
    ensurePresetExists(presets.testPresets, preset, "Test");
    const args = ["--preset", preset, "--output-on-failure"];
    if (filter) {
      args.push("-R", filter);
    }
    return textResponse(formatCommandResult(await runAllowedCommand("ctest", args)));
  }
);

server.tool(
  "run_test_target",
  "Build and run one current Web++ test target derived from tests/*_test.cpp, including tests without a dedicated preset.",
  {
    target: z.string().min(1).describe("Focused test target returned by list_test_targets."),
    configurePreset: z
      .string()
      .min(1)
      .default("dev-default")
      .describe("Configure preset whose build directory should be used.")
  },
  async ({ target, configurePreset }) => {
    const testTargets = await listTestTargets();
    if (!testTargets.includes(target)) {
      throw new Error(`Focused test target does not exist: ${target}`);
    }

    const presets = await readCMakePresets();
    const buildDir = resolveConfigureBuildDir(presets, configurePreset);

    const build = await runAllowedCommand("cmake", ["--build", buildDir, "--target", target]);
    if (build.exitCode !== 0) {
      return textResponse(formatCommandResult(build));
    }

    const test = await runAllowedCommand("ctest", [
      "--test-dir",
      buildDir,
      "--output-on-failure",
      "-R",
      `^${escapeRegularExpression(target)}$`
    ]);
    return textResponse(`${formatCommandResult(build)}\n\n${formatCommandResult(test)}`);
  }
);

server.tool(
  "run_all_tests",
  "Build every current tests/*_test.cpp target and run the complete CTest suite without relying on the manually enumerated tests build preset.",
  {
    configurePreset: z
      .string()
      .min(1)
      .default("dev-default")
      .describe("Configure preset whose build directory should be used.")
  },
  async ({ configurePreset }) => {
    const presets = await readCMakePresets();
    const buildDir = resolveConfigureBuildDir(presets, configurePreset);
    const testTargets = await listTestTargets();
    const build = await runAllowedCommand("cmake", ["--build", buildDir, "--target", ...testTargets]);
    if (build.exitCode !== 0) {
      return textResponse(formatCommandResult(build));
    }

    const test = await runAllowedCommand("ctest", ["--test-dir", buildDir, "--output-on-failure"]);
    return textResponse(`${formatCommandResult(build)}\n\n${formatCommandResult(test)}`);
  }
);

server.tool(
  "resolve_build_directory",
  "Resolve and validate the build directory associated with a Web++ configure preset.",
  {
    preset: z.string().min(1).default("dev-default").describe("Configure preset from CMakePresets.json.")
  },
    async ({ preset }) => {
      const presets = await readCMakePresets();
      const resolved = resolveConfigureBuildDir(presets, preset);
      return textResponse(`preset: ${preset}\nbuild_directory: ${relativeProjectPath(resolved)}`);
    }
  );

async function main(): Promise<void> {
  await server.connect(new StdioServerTransport());
}

main().catch((error: unknown) => {
  console.error(error);
  process.exit(1);
});
