---
id: ts-csolution-local-pack-path-schema
type: troubleshooting
title: "csolution: schema check failed for a packs entry with path: but without pack:"
summary: A local pack directory is referenced in csolution packs: with both pack: Vendor::Name and path:; a bare path: entry fails the schema check.
domain: project
status: draft
tool: csolution
severity: error
symptoms:
  - "error csolution: schema check failed, verify syntax"
symptom_regex: "schema check failed, verify syntax"
applies_to:
  tool: [csolution, cbuild]
source: own observation with CMSIS-Toolbox 2.15.0; etc/common.schema.json PackPathType
license: Apache-2.0
confidence: high
---

## Symptom

```
Blinky.csolution.yml:13:7 - error csolution: schema check failed, verify syntax
```

at a `packs:` entry written as `- path: ../..`.

## Cause

The schema type `PackPathType` requires both `pack` (versionless `Vendor::Name`) and `path`. The vendor and name
must match the `.pdsc` in that directory.

## Fix

```yaml
  packs:
    - pack: MicroBit::MicroBit_V2_BSP      # pack under development, not installed
      path: ../..
    - pack: NordicSemiconductor::nRF_DeviceFamilyPack@^8.44.1
```

The toolbox then warns that an absolute `path` is not portable; keep it relative. This is the way to build the
examples of a pack from its source tree before the pack is packaged and installed. Remove the `path:` line for the
shipped example.
