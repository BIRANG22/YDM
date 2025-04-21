using UnityEngine;
using System.Collections;
public enum PlayerType
{
    Player1, 
    Player2  
}

public class PlayerController : MonoBehaviour
{
    public float jumpForce = 10f;           //점프파워
    public float moveSpeed = 10f;           //이동속도
    public float dashSpeed = 20f;       // 대시 속도
    public float dashDuration = 0.2f;   // 대시 지속 시간
    public float dashCooldown = 1f;     // 대시 쿨타임
    public PlayerType playerType; // Player1 / Player2

    private Rigidbody2D rb;
    private bool isDashing = false;
    private float dashTimeLeft;
    private float lastDashTime = -Mathf.Infinity;

    void Start()
    {
        rb = GetComponent<Rigidbody2D>();

        if (playerType == PlayerType.Player2)
        {
            FlipUpsideDown();
        }
    }
    
    public void Jump()
    {
        float jumpDirection = (playerType == PlayerType.Player2) ? -1f : 1f;  
        rb.linearVelocity = new Vector2(rb.linearVelocity.x, jumpForce * jumpDirection);
    }
    public void Move(float horizontalInput)
    {
        Debug.Log("이동");
        float moveValue = horizontalInput * moveSpeed;

        rb.linearVelocity = new Vector2(moveValue, rb.linearVelocity.y);

        // 이동 방향에 따라 좌우 반전 처리
        if (moveValue != 0)
        {
            Vector3 scale = transform.localScale;
            scale.x = Mathf.Sign(moveValue) * Mathf.Abs(scale.x);
            transform.localScale = scale;
        }
    }

    public void Dash()
    {
        // 쿨타임 체크
        if (Time.time < lastDashTime + dashCooldown || isDashing)
            return;

        StartCoroutine(HandleDash());
    }

    private IEnumerator HandleDash()
    {
        isDashing = true;
        lastDashTime = Time.time;
        dashTimeLeft = dashDuration;

        float direction = transform.localScale.x > 0 ? 1f : -1f;
        rb.linearVelocity = new Vector2(direction * dashSpeed, rb.linearVelocity.y);

        while (dashTimeLeft > 0)
        {
            dashTimeLeft -= Time.deltaTime;
            yield return null;
        }

        rb.linearVelocity = Vector2.zero;
        isDashing = false;
    }

    void FlipUpsideDown()
    {
        transform.localScale = new Vector3(transform.localScale.x, -transform.localScale.y, transform.localScale.z);
    }
    public void SetGravityInverted(bool inverted)
    {
        if (inverted)
        {
            transform.localScale = new Vector3(transform.localScale.x, -transform.localScale.y, transform.localScale.z);
        }
    }
}