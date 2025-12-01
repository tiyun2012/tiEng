<#
.SYNOPSIS
    Basic geometry helper functions (circle, rectangle, triangle).

.DESCRIPTION
    Provides simple functions to calculate area and perimeter/metrics for common shapes.
#>

#region Circle
function Get-CircleMetrics {
        [CmdletBinding()]
        param(
                [Parameter(Mandatory)]
                [ValidateScript({ $_ -gt 0 })]
                [double]$Radius
        )

        $area = [Math]::PI * $Radius * $Radius
        $perimeter = 2 * [Math]::PI * $Radius

        [PSCustomObject]@{
                Shape     = 'Circle'
                Radius    = $Radius
                Area      = [Math]::Round($area, 6)
                Perimeter = [Math]::Round($perimeter, 6)
        }
}
#endregion

#region Rectangle
function Get-RectangleMetrics {
        [CmdletBinding()]
        param(
                [Parameter(Mandatory)]
                [ValidateScript({ $_ -gt 0 })]
                [double]$Width,

                [Parameter(Mandatory)]
                [ValidateScript({ $_ -gt 0 })]
                [double]$Height
        )

        $area = $Width * $Height
        $perimeter = 2 * ($Width + $Height)

        [PSCustomObject]@{
                Shape     = 'Rectangle'
                Width     = $Width
                Height    = $Height
                Area      = [Math]::Round($area, 6)
                Perimeter = [Math]::Round($perimeter, 6)
        }
}
#endregion

#region Triangle (area by base & height; optional perimeter if sides provided)
function Get-TriangleMetrics {
        [CmdletBinding()]
        param(
                [Parameter(Mandatory)]
                [ValidateScript({ $_ -gt 0 })]
                [double]$Base,

                [Parameter(Mandatory)]
                [ValidateScript({ $_ -gt 0 })]
                [double]$Height,

                [ValidateScript({ $_ -gt 0 })]
                [double]$SideA = $null,

                [ValidateScript({ $_ -gt 0 })]
                [double]$SideB = $null,

                [ValidateScript({ $_ -gt 0 })]
                [double]$SideC = $null
        )

        $area = 0.5 * $Base * $Height

        $perimeter = $null
        if ($PSBoundParameters.ContainsKey('SideA') -and $PSBoundParameters.ContainsKey('SideB') -and $PSBoundParameters.ContainsKey('SideC')) {
                $perimeter = $SideA + $SideB + $SideC
        }

        [PSCustomObject]@{
                Shape     = 'Triangle'
                Base      = $Base
                Height    = $Height
                Area      = [Math]::Round($area, 6)
                Perimeter = if ($perimeter) { [Math]::Round($perimeter,6) } else { $null }
        }
}
#endregion

# Demo when the script is executed directly
if ($PSCommandPath -and $PSCommandPath -eq $MyInvocation.MyCommand.Path) {
        "Demo: Geometry metrics"
        Get-CircleMetrics -Radius 3 | Format-List
        Get-RectangleMetrics -Width 4 -Height 5 | Format-List
        Get-TriangleMetrics -Base 6 -Height 4 -SideA 3 -SideB 4 -SideC 5 | Format-List
}